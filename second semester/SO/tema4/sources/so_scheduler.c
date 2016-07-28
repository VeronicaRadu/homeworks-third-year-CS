/* Rotaru Alexandru Andrei 335CC*/

#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <semaphore.h>
#include <unistd.h>

#include "../headers/utils.h"
#include "../headers/debug.h"
#include "../headers/priority_queue.h"
#include "../headers/so_scheduler.h"
#include "../headers/task_chunk.h"
#include "../headers/list.h"

typedef struct _scheduler {
	unsigned max_time_quanta;
	unsigned max_io_devs_cnt;
	list_node_t io_devs[SO_MAX_NUM_EVENTS];
	priority_queue_t ready;
	list_node_t terminated;
	task_chunk_t *running;
	unsigned curr_time_quanta;
	unsigned time_stamp;
	unsigned thread_cnt;
	pthread_mutex_t gsync;
	pthread_cond_t gcond;
} scheduler_t;

static scheduler_t scheduler;

unsigned int tabs = 0;

/*
 * This function is going to schedule the next thread on the cpu. If the current running thread is
 * nil then takes the top one from the queue, if not, then compares the priorities and the
 * time quanta of the running thread and accordint to them schedule a thread from the queue or
 * reschedules the current thread
 */

static void schedule(void)
{
	task_chunk_t *preemted	= NULL;
	task_chunk_t *top_ready = NULL;

	TRACE("Scheduling", " ");

	DIE(0 != pthread_mutex_lock(&scheduler.gsync), "Error locking the global mutex");
	top_ready = (task_chunk_t *)top(&scheduler.ready);
	DIE(0 != pthread_mutex_unlock(&scheduler.gsync), "Error unlocking the global mutex");

	if ((NULL == scheduler.running && NULL == top_ready) || NULL == top_ready) {
		/*both ready queue and executing thread are null, nothing to be done*/
		return;
	} else if (NULL == scheduler.running) {
		/*need to put another thread here*/
		scheduler.running = top_ready;
		pop(&scheduler.ready);
		scheduler.curr_time_quanta = scheduler.max_time_quanta;
		DIE(0 != sem_post(&scheduler.running->sem), "Error posting to unlock the thread");
	} else if ((0 == scheduler.curr_time_quanta
		&& scheduler.running->spriority <= top_ready->spriority)
		|| (0 != scheduler.curr_time_quanta
		&& scheduler.running->spriority < top_ready->spriority)) {
		/*exchange*/

		DIE(0 != pthread_mutex_lock(&scheduler.gsync), "Error locking the global mutex");

		pop(&scheduler.ready);
		preemted = scheduler.running;
		preemted->time_stamp = scheduler.time_stamp++;
		scheduler.curr_time_quanta = scheduler.max_time_quanta;
		insert(preemted)into(&scheduler.ready);
		scheduler.running = top_ready;

		DIE(0 != pthread_mutex_unlock(&scheduler.gsync), "Error unlocking the global mutex");

		DIE(0 != sem_post(&scheduler.running->sem), "Error posting to unlock the thread");
		DIE(0 != sem_wait(&preemted->sem), "Error waiting the semaphore of preemted thread");
	} else if (0 == scheduler.curr_time_quanta)
		scheduler.curr_time_quanta = scheduler.max_time_quanta;

	UNTRACE();
}

int so_init(unsigned time_slice, unsigned io_devs_cnt)
{
	if (0 == time_slice  || SO_MAX_NUM_EVENTS < io_devs_cnt)
		return -1;

	if (scheduler.max_time_quanta == time_slice && scheduler.max_io_devs_cnt == io_devs_cnt)
		return -1;

	scheduler.max_time_quanta = time_slice;
	scheduler.max_io_devs_cnt = io_devs_cnt;
	scheduler.time_stamp = 0;
	scheduler.thread_cnt = 0;
	scheduler.gcond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
	scheduler.gsync = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	scheduler.terminated.next = NULL;
	scheduler.terminated.info = NULL;
	scheduler.ready = new_priority_queue(50, compare_thread_priority, compare_thread_id);

	int i = 0;
	for (i = 0; i < SO_MAX_NUM_EVENTS; ++i) {
		scheduler.io_devs[i].info = NULL;
		scheduler.io_devs[i].next = NULL;
	}
	return 0;
}

/*
 * Function that wrappers the handler executed by the thread, and waits after the semaphore of the task
 */
static void *start_thread(void *arg)
{
	task_chunk_t *tsk = NULL;

	TRACE("StartThread", "%p", arg);

	tsk = (task_chunk_t *)arg;
	DIE(0 != sem_wait(&tsk->sem), "Error wating the signal form schduler");
	tsk->handler(tsk->spriority);

	DIE(0 != pthread_mutex_lock(&scheduler.gsync), "Error locking the global mutex");
	--scheduler.thread_cnt;
	add(tsk)to(&scheduler.terminated);
	scheduler.running = NULL;
	scheduler.curr_time_quanta = 0;
	if (0 == scheduler.thread_cnt)
		DIE(0 != pthread_cond_signal(&scheduler.gcond), "Error signaling the joining of the threads");
	DIE(0 != pthread_mutex_unlock(&scheduler.gsync), "Error unlocking the global mutex");

	schedule();
	UNTRACE();
	return NULL;
}

tid_t so_fork(so_handler *handler, unsigned priority)
{
	sem_t sem;
	tid_t tid = INVALID_TID;
	task_chunk_t *tsk = NULL;
	int rc = 0;

	scheduler.curr_time_quanta--;
	TRACE("SO_FORK", "%p, %u", handler, priority);
	if (NULL == handler || priority > SO_MAX_PRIO)
		return tid;

	sem_init(&sem, 0, 0);

	tsk = new_task_chunk(tid, priority, handler, sem);
	rc = pthread_create(&tsk->tid, NULL, start_thread, tsk);
	if (0 != rc)
		return INVALID_TID;
	tid = tsk->tid;


	DIE(0 != pthread_mutex_lock(&scheduler.gsync), "Error locking the global mutex");
	++scheduler.thread_cnt;
	tsk->time_stamp = scheduler.time_stamp++;
	insert(tsk)into(&scheduler.ready);
	DIE(0 != pthread_mutex_unlock(&scheduler.gsync), "Error unlocking the global mutex");

	static unsigned first = 0;
	if (first == 0) {
		first = 1;

		DIE(0 != pthread_mutex_lock(&scheduler.gsync), "Error locking the global mutex");
		scheduler.curr_time_quanta = scheduler.max_time_quanta;
		scheduler.running = (task_chunk_t *)top(&scheduler.ready);
		pop(&scheduler.ready);
		DIE(0 != pthread_mutex_unlock(&scheduler.gsync), "Error unlocking the global mutex");

		DIE(-1 == sem_post(&scheduler.running->sem), "Error auto-signaling the first thread");
	} else
		schedule();

	UNTRACE();
	return tid;
}

int so_wait(unsigned dev_idx)
{
	task_chunk_t *waiting = NULL;

	TRACE("SO_WAIT", "%u", dev_idx);

	scheduler.curr_time_quanta--;
	if (dev_idx >= scheduler.max_io_devs_cnt) {
		schedule();
		return -1;
	}
	waiting = scheduler.running;
	scheduler.running = NULL;

	add(waiting)to(&scheduler.io_devs[dev_idx]);
	schedule();
	DIE(0 != sem_wait(&waiting->sem), "Error waiting the semaphor in so_wait");
	UNTRACE();
	return 0;
}

int so_signal(unsigned dev_idx)
{
	unsigned tasks_cnt = 0;
	list_node_t *iterator = NULL;
	list_node_t *piterator = NULL;

	TRACE("SO_SIGNAL", "%u", dev_idx);

	scheduler.curr_time_quanta--;
	if (dev_idx >= scheduler.max_io_devs_cnt) {
		schedule();
		return -1;
	}

	for (iterator = scheduler.io_devs[dev_idx].next; NULL != iterator; iterator = iterator->next) {
		if (NULL != piterator)
			free(piterator);
		insert(iterator->info)into(&scheduler.ready);
		piterator = iterator;
		tasks_cnt++;
	}
	scheduler.io_devs[dev_idx].next = NULL;
	schedule();
	UNTRACE();
	return tasks_cnt;
}

void so_exec(void)
{
	TRACE("SO_EXEC", " ");
	scheduler.curr_time_quanta--;
	schedule();
	UNTRACE();
}

void so_end(void)
{
	list_node_t *iterator = NULL;

	DIE(0 != pthread_mutex_lock(&scheduler.gsync), "Error locking the global mutex");
	while (0 != scheduler.thread_cnt)
		DIE(0 != pthread_cond_wait(&scheduler.gcond, &scheduler.gsync), "Error waiting after the signal");
	DIE(0 != pthread_mutex_unlock(&scheduler.gsync), "Error unlocking the global mutex");

	iterator = scheduler.terminated.next;
	for (;iterator != NULL; iterator = iterator->next) {
		task_chunk_t *curr_thread = (task_chunk_t *)iterator->info;
		DIE(0 != pthread_join(curr_thread->tid, NULL), "Error joining a thread");
		free(curr_thread);
		free(iterator);
	}

	del_priority_queue(&scheduler.ready);
	scheduler.max_time_quanta = 0;
	scheduler.max_io_devs_cnt = 0;
}

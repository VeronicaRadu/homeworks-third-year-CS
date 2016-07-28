"""
Work pool that receives concurently the data for processing and process it using
a fixed number of threads. The implementation offers an api to sync with the
pool if it is in a idle state using method finish_work()
"""

from threading import Lock, Event, Semaphore, Thread

class WorkerThread(Thread):
    """
    Worker thread is a used in a work pool as working entity and synchronizes
    with the other working threads through the sync elements provided in the
    belonging workpool.
    """
    def __init__(self, parent_work_pool):
        """
        The name can be used for debug.
        The pool provides the data for processing along with sync methods
        """
        Thread.__init__(self, name="WorkerThread")
        self.pool = parent_work_pool

    def run(self):
        """
        The main loop of the worker in which if therea are any data for
        processing the worker takes and process it. As long as at least one of
        the threads are running the workpool can not be stoped.
        """
        while True:
            self.pool.task_sign.acquire()
            if self.pool.stop:
                break
            current_task = None
            with self.pool.task_lock:
                task_count = len(self.pool.tasks_list)
                # if we have multiple task to do
                if task_count > 0:
                    current_task = self.pool.tasks_list[0]
                    self.pool.tasks_list = self.pool.tasks_list[1:]
                # if we process the last one
                if task_count == 1:
                    self.pool.no_tasks.set()

            if current_task is not None:
                (current_device, script, location, neighbourhood) = current_task
                with current_device.glocks[location]:
                    common_data = []
                    # collect data from current neighbours
                    for neighbour in neighbourhood:
                        data = neighbour.get_data(location)
                        if data is not None:
                            common_data.append(data)
                    # add our data, if any
                    data = current_device.get_data(location)
                    if data is not None:
                        common_data.append(data)

                    if common_data != []:
                         # run script on data
                        result = script.run(common_data)
                        for neighbour in neighbourhood:
                            neighbour.set_data(location, result)
                        # update our data, can be done in parallel
                        current_device.set_data(location, result)

class WorkPool(object):
    """
    WorkPool is class that runs along with execution of the program and process
    the scripts in a parallel manner.
    """
    def __init__(self, size):
        self.size = size

        self.tasks_list = [] # (device, script, location, neighbours)
        self.task_lock = Lock()
        self.task_sign = Semaphore(0)
        self.no_tasks = Event()
        self.no_tasks.set()
        self.stop = False

        self.workers = []
        for i in xrange(self.size):
            worker = WorkerThread(self)
            self.workers.append(worker)

        for worker in self.workers:
            worker.start()

    def work(self, task):
        """
        Add a new task as a list of the device invoking, the script,
        the location and the current neighbourhood of the device at
        that moment.
        """
        with self.task_lock:
            self.tasks_list.append(task)
            self.task_sign.release()
            if self.no_tasks.is_set():
                self.no_tasks.clear()

    def finish_work(self):
        """
        Blocks all the calling threads untill there is not other task to be
        procesed. Used for synchronization of the device threads with the
        working pool.
        """
        self.no_tasks.wait()

    def end(self):
        """
        Signal the termination of the work pool and joins its threads for a
        gracefull ending.
        """
        self.finish_work()
        self.stop = True
        for thread in self.workers:
            self.task_sign.release()
        for thread in self.workers:
            thread.join()

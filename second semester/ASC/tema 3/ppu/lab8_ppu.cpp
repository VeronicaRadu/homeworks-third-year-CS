#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <libspe2.h>
#include <pthread.h>
#include <libmisc.h>
#include "../lab8_common.h"
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>


extern spe_program_handle_t lab8_spu;

int _open_for_read(char* path){
	int fd;

	fd = open(path, O_RDONLY);
	if (fd < 0){
		fprintf(stderr, "%s: Error opening %s\n", __func__, path);
		exit(0);
	}
	return fd;
}

int _open_for_write(char* path){
	int fd;

	fd = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (fd < 0){
		fprintf(stderr, "%s: Error opening %s\n", __func__, path);
		exit(0);
	}
	return fd;
}

void _write_file(char* filepath, void* buf, int size){
	char *ptr;
	int left_to_write, bytes_written, fd;

	fd = _open_for_write(filepath);

	ptr = (char*)buf;
	left_to_write = size;

	while (left_to_write > 0){
		bytes_written = write(fd, ptr, left_to_write);
		if (bytes_written <= 0){
			fprintf(stderr, "%s: Error writing buffer. "
					"fd=%d left_to_write=%d size=%d bytes_written=%d\n", 
					__func__, fd, left_to_write, size, bytes_written);
			exit(0);
		}
		left_to_write -= bytes_written;
		ptr += bytes_written;
	}

	close(fd);

}

void* _read_file(char* filepath, int* size_ptr){
	char *ptr;
	int left_to_read, bytes_read, size, fd;
	void* buf;

	fd = _open_for_read(filepath);

	size = lseek(fd, 0, SEEK_END);
	if (size <= 0) {
		fprintf(stderr, "%s: Error getting file size. filepath=%s\n",
				__func__, filepath);
		exit(0);
	}
	buf = malloc(size);
	//buf = malloc_align(size, 4);
	if (!buf) {
		fprintf(stderr, "%s: Error allocating %d bytes\n", __func__,
				size);
		exit(0);   
	}
	lseek(fd, 0, SEEK_SET);

	ptr = (char*) buf;
	left_to_read = size;
	while (left_to_read > 0){
		bytes_read = read(fd, ptr, left_to_read);
		if (bytes_read <= 0){
			fprintf(stderr, "%s: Error reading buffer. "
					"fd=%d left_to_read=%d size=%d bytes_read=%d\n", 
					__func__, fd, left_to_read, size, bytes_read);
			exit(0);
		}
		left_to_read -= bytes_read;
		ptr += bytes_read;
	}

	close(fd);
	*size_ptr = size;

	return buf;
}


void *ppu_pthread_function(void *thread_arg) {

	//printf("In a thread\n");
	spe_context_ptr_t ctx;
	message_t *arg = (message_t *) thread_arg;

	/* Create SPE context */
	if ((ctx = spe_context_create (0, NULL)) == NULL) {
		perror ("Failed creating context");
		exit (1);
	}
	//printf("Created spe-context\n");
	/* Load SPE program into context */
	if (spe_program_load (ctx, &lab8_spu)) {
		perror ("Failed loading program");
		exit (1);
	}
	//printf("Load program\n");
	/* Run SPE context */
	unsigned int entry = SPE_DEFAULT_ENTRY;
	//printf("Should send the data now\n");
	if (spe_context_run(ctx, &entry, 0, (void*)arg, NULL, NULL) < 0) {  
		perror ("Failed running context");
		exit (1);
	}
	//printf("Runned context\n");
	/* Destroy context */
	if (spe_context_destroy (ctx) != 0) {
		perror("Failed destroying context");
		exit (1);
	}
	//printf("Destroyed context\n");

	pthread_exit(NULL);
}

void process_single (int spu_cnt, int vectorial, int dma, char op, char* in, char* key, char* out) {
	struct timeval t1, t2, t3, t4;
	double total_time = 0, cpu_time = 0;
	unsigned int *buf = NULL;
	unsigned int *key_buf = NULL;
	unsigned int *out_buf = NULL;
	int in_size;
	int key_size;
	int num_ints = 256;
	int chunk_size = 0;
	int operation = (op == 'e' ? 1 : 0);
	//spu_cnt -= '0';
	//printf("Spu_cnt is %d\n", (int)spu_cnt);
	message_t thread_arg[8] __attribute__ ((aligned(16)));
	pthread_t thread_ids[8] __attribute__ ((aligned(16)));

	//spu_cnt -= '0';// turn it into real number

	gettimeofday(&t3, NULL);
	in = in;
	key = key;	
	out = out;
	 
	buf = (unsigned int*) _read_file(in, &in_size);
	key_buf = (unsigned int*) _read_file(key, &key_size);

	unsigned int *all_key = (unsigned int *)malloc_align(key_size, 4);
	unsigned int *all_buf = (unsigned int *)malloc_align(in_size, 4);

	memcpy(all_key, key_buf, key_size);
	memcpy(all_buf, buf, in_size);

	if (key_size != 4 * sizeof(int)) {
		printf("Invalid key file %s\n", key);
		return;
	}	
	
	gettimeofday(&t1, NULL);
	num_ints = in_size / sizeof(int);
	chunk_size = num_ints / (unsigned int) spu_cnt;
	while (in_size / spu_cnt < 4)
		spu_cnt /= 2;
	for(short i = 0; i < spu_cnt; ++i) {
		thread_arg[i].data_ptr = all_buf + i * chunk_size;
		thread_arg[i].key_ptr = all_key;
		thread_arg[i].data_length = chunk_size;
		thread_arg[i].vectorial = vectorial;
		thread_arg[i].dma = dma;
		thread_arg[i].op = operation;
		thread_arg[i].spu_cnt = spu_cnt;
		//printf("Creating thread#%d\n", i);
		/* Create thread for each SPE context */
		if (pthread_create (&thread_ids[i], NULL, &ppu_pthread_function, &thread_arg[i]))  {
			perror ("Failed creating thread");
			exit (1);
		}
	}
	/* Wait for SPU-thread to complete execution.  */
	for (char i = 0; i < spu_cnt; i++) {
		//printf("Waiting the thread#%d\n", i);
		if (pthread_join (thread_ids[i], NULL)) {
			perror("Failed pthread_join");
			exit (1);
		}
	}
	memcpy(buf, all_buf, in_size);
	gettimeofday(&t2, NULL);

	_write_file(out, buf, in_size);
	free(buf); 
	free(key_buf);
	gettimeofday(&t4, NULL);

	total_time += GET_TIME_DELTA(t3, t4);
	cpu_time += GET_TIME_DELTA(t1, t2);

	printf("%d %lf\n", in_size, cpu_time);
	//if (op == 'e') 
	//	printf("Encrypted[%d] in [CPU=%lf, Total=%lf]\n", in, 
	//			in_size, cpu_time, total_time);
	//else
	//	printf("Decrypted [%s,%d] in [CPU=%lf, Total=%lf]\n", in, 
	//			in_size, cpu_time, total_time);
}

void process_multi (short spu_cnt, short vectorial, char dma, char* input) {
	FILE* fptr = fopen(input, "r");
	char op, in[PATH_SIZE], key[PATH_SIZE], out[PATH_SIZE];  
	if (!fptr) {
		fprintf(stderr, "Can't open %s\n", input);
		return;
	}
	while (fscanf(fptr, "%c %s %s %s\n", &op, in, key, out)) {
		process_single(spu_cnt, vectorial, dma, op, in, key, out);
		if (feof(fptr)) break;
	}

	fclose(fptr);

}


int main(int argc, char **argv)
{
	if (argc == 8) {
		process_single(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), argv[4][0], argv[5], argv[6], argv[7]);
		return 0;
	} else if (argc == 5) {
		process_multi(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), argv[4]);
		return 0;
	}
	printf("Usage: %s num_spus mod_vect mod_dma e/d in key out\n", argv[0]);

	return 0;
}

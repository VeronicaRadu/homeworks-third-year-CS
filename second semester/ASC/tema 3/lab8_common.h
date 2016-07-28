typedef struct {
	unsigned int *data_ptr;
	unsigned int *key_ptr;
	unsigned int data_length;
	unsigned int vectorial;
	unsigned int dma;
	unsigned int spu_cnt;
	unsigned op;
	char options[4];
} message_t;

#define NUM_SPU_THREADS   (8)
#define DMA_TRANSFER_SIZE (16 * 1024)
#define SPU_ARR_SIZE      (DMA_TRANSFER_SIZE / sizeof(float))
//#define PPU_ARR_SIZE      (32768)
//#define PPU_ARR_SIZE      (131072)
//

#define DELTA    0x9e3779b9
#define GET_TIME_DELTA(t1, t2) ((t2).tv_sec - (t1).tv_sec + \
                ((t2).tv_usec - (t1).tv_usec) / 1000000.0)
#ifdef DEBUG
#define NUM_ROUNDS 1
#define DPRINT printf
#define DEC_SUM DELTA
#else
#define DPRINT(...) 
#define NUM_ROUNDS 32
#define DEC_SUM 0xC6EF3720
#endif

#define LINE_SIZE 128
#define PATH_SIZE 40

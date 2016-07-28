#include <stdio.h>
#include <string.h>
#include <spu_intrinsics.h>
#include <spu_mfcio.h>
#include "../lab8_common.h"

#include <stdlib.h>
#include <spu_intrinsics.h>
#include <errno.h>
#include <spu_mfcio.h>
//#include <memalign.h>
#include <libmisc.h>


/* TODO: adaugati define-ul de wait_tag (vezi exemple DMA) */
#define wait_tag(t) mfc_write_tag_mask(1<<t); mfc_read_tag_status_all();

void encrypt_block (unsigned int* v, unsigned int* k) {
	unsigned int i, sum = 0;

	for (i=0; i < NUM_ROUNDS; i++) {                     
		sum += DELTA;
		v[0] += ((v[1]<<4) + k[0]) ^ (v[1] + sum) ^ ((v[1]>>5) + k[1]);
		v[1] += ((v[0]<<4) + k[2]) ^ (v[0] + sum) ^ ((v[0]>>5) + k[3]);
		DPRINT("\tround = %d sum = %08x v0 = %08x v1 = %08x\n", i, sum, v[0], v[1]);	
	}                               
}

void decrypt_block (unsigned int* v, unsigned int* k) {
	unsigned int sum = DEC_SUM, i;
	for (i=0; i<NUM_ROUNDS; i++) { 
		v[1] -= ((v[0]<<4) + k[2]) ^ (v[0] + sum) ^ ((v[0]>>5) + k[3]);
		v[0] -= ((v[1]<<4) + k[0]) ^ (v[1] + sum) ^ ((v[1]>>5) + k[1]);
		sum -= DELTA;
		DPRINT("\tround = %d sum = %08x v0 = %08x v1 = %08x\n", i, sum, v[0], v[1]);	
	}                             
}

void encrypt_vector(unsigned int *v, unsigned int *k, int num_ints)
{
	unsigned int i;
	unsigned int v_pos_even[num_ints / 2];
	unsigned int v_pos_odd[num_ints / 2];
	for (i = 0; i < num_ints / 2; ++i) {
		v_pos_even[i] = v[i * 2];
		v_pos_odd[i] = v[i * 2 + 1];
	}

	
	vector unsigned int *vect_pos_par = (vector unsigned int *)v_pos_even;
	vector unsigned int *vect_pos_imp = (vector unsigned int *)v_pos_odd;
	vector unsigned int vect_k0 = spu_splats(k[0]);
	vector unsigned int vect_k1 = spu_splats(k[1]);
	vector unsigned int vect_k2 = spu_splats(k[2]);
	vector unsigned int vect_k3 = spu_splats(k[3]);
	vector unsigned int vect_sum = spu_splats(0u);
	vector unsigned int vect_del = spu_splats(DELTA);
	for (i=0; i<NUM_ROUNDS; i++) {
		vect_sum += vect_del;
		for (int idx = 0; idx < num_ints / 8; ++idx) {
                	vect_pos_par[idx] += (spu_sl(vect_pos_imp[idx], 4) + vect_k0) ^ (vect_pos_imp[idx] + vect_sum) ^ (spu_rlmask(vect_pos_imp[idx], -5) + vect_k1);
                	vect_pos_imp[idx] += (spu_sl(vect_pos_par[idx], 4) + vect_k2) ^ (vect_pos_par[idx] + vect_sum) ^ (spu_rlmask(vect_pos_par[idx], -5) + vect_k3);
		}
	}
	
	for (i = 0; i < num_ints / 2; ++i) {
		v[i * 2] = v_pos_even[i];
		v[i * 2 + 1] = v_pos_odd[i]; 
	}
}

void decrypt_vector(unsigned int* v, unsigned int* k, int num_ints) {
	unsigned int i;
        unsigned int v_pos_even[num_ints / 2];
        unsigned int v_pos_odd[num_ints / 2];
        for (i = 0; i < num_ints / 2; ++i) {
                v_pos_even[i] = v[i * 2];
                v_pos_odd[i] = v[i * 2 + 1];
        }


        vector unsigned int *vect_pos_par = (vector unsigned int *)v_pos_even;
        vector unsigned int *vect_pos_imp = (vector unsigned int *)v_pos_odd;
        vector unsigned int vect_k0 = spu_splats(k[0]);
        vector unsigned int vect_k1 = spu_splats(k[1]);
        vector unsigned int vect_k2 = spu_splats(k[2]);
        vector unsigned int vect_k3 = spu_splats(k[3]);
        vector unsigned int vect_sum = spu_splats(DEC_SUM);
        vector unsigned int vect_del = spu_splats(DELTA);
        for (i=0; i<NUM_ROUNDS; i++) {
                for (int idx = 0; idx < num_ints / 8; ++idx) {       
                        vect_pos_imp[idx] -= (spu_sl(vect_pos_par[idx], 4) + vect_k2) ^ (vect_pos_par[idx] + vect_sum) ^ (spu_rlmask(vect_pos_par[idx], -5) + vect_k3);
			vect_pos_par[idx] -= (spu_sl(vect_pos_imp[idx], 4) + vect_k0) ^ (vect_pos_imp[idx] + vect_sum) ^ (spu_rlmask(vect_pos_imp[idx], -5) + vect_k1);
                }
		vect_sum -= vect_del;
        }

        for (i = 0; i < num_ints / 2; ++i) {
                v[i * 2] = v_pos_even[i];
                v[i * 2 + 1] = v_pos_odd[i];
        }
}

void encrypt (unsigned int* v, unsigned int* k, int num_ints) {
	DPRINT("encrypt: num_ints = %d\n", num_ints);
	for (int i = 0; i < num_ints; i += 2) { 
		DPRINT("encrypt: block = %d\n", i / 2);
		encrypt_block(&v[i], k);
	}
}

void decrypt (unsigned int* v, unsigned int* k, int num_ints)  {
	DPRINT("decrypt: num_ints = %d\n", num_ints);
	for (int i = 0; i < num_ints; i += 2) {
		DPRINT("decrypt: block = %d\n", i / 2);
		decrypt_block(&v[i], k);
	}
}
const unsigned int BUF_LEN = 16 * 1024 / sizeof(int);

static int min(int a, int b){
	return (a < b ? a : b);
}

int main(unsigned long long speid, unsigned long long argp, unsigned long long envp)
{
	uint32_t tag_id[2];
	tag_id[0] = mfc_tag_reserve();
	if (tag_id[0] == MFC_TAG_INVALID){
		printf("SPU: ERROR can't allocate tag ID\n"); 
		return -1;
	}

	tag_id[1] = mfc_tag_reserve();
        if (tag_id[1] == MFC_TAG_INVALID){
                printf("SPU: ERROR can't allocate tag ID\n"); 
                return -1;
        }
	message_t m;

	mfc_get((void*)&m, argp, sizeof(message_t), tag_id[0], 0, 0);
	wait_tag(tag_id[0]);
		
	unsigned int key[4] 		__attribute__ ((aligned(16)));
	unsigned int data[2][BUF_LEN] 	__attribute__ ((aligned(16)));
	
	mfc_get((void*)key, m.key_ptr, 4 * sizeof(unsigned int), tag_id[0], 0, 0);
	wait_tag(tag_id[0]);
	
	int bytes_sent = 0;
	int len = min(BUF_LEN, m.data_length - bytes_sent);
	if (1 == m.dma) {
		int buf = 0;
		int nxt_buf = 0;
		mfc_get((void*)data[buf], (uint32_t)m.data_ptr, len * sizeof(int), tag_id[buf], 0, 0);
		int i = 1;
		while(i < m.data_length / BUF_LEN) {
			nxt_buf = buf ^ 1;
			wait_tag(tag_id[nxt_buf]);
			mfc_get((void *)data[nxt_buf], (uint32_t)m.data_ptr + i * len * sizeof(int), len * sizeof(int), tag_id[nxt_buf], 0, 0);
			wait_tag(tag_id[buf]);
			if (1 == m.vectorial) {
                                if (1 == m.op)
                                        encrypt_vector(data[buf], key, len);
                                else
                                        decrypt_vector(data[buf], key, len);

                        } else {
                                if (1 == m.op)
                                        encrypt(data[buf], key, len);
                                else
                                        decrypt(data[buf], key, len);
                        }
			mfc_put((void*)data[buf], (uint32_t)m.data_ptr + (i - 1) * len * sizeof(int), len * sizeof(int), tag_id[buf], 0, 0);
			buf = nxt_buf;
			i++;
		}
		wait_tag(tag_id[buf]);
		if (1 == m.vectorial) {
		        if (1 == m.op)
                        	encrypt_vector(data[buf], key, len);
                        else
                                decrypt_vector(data[buf], key, len);
                } else {
                        if (1 == m.op)
                                encrypt(data[buf], key, len);
                        else
                                decrypt(data[buf], key, len);
                }
		mfc_put((void *)data[buf], (uint32_t)m.data_ptr + (i - 1) * len * sizeof(int), len * sizeof(int), tag_id[buf], 0, 0);
		wait_tag(tag_id[buf]);
	} else {
		for (int i = 0; i < m.data_length / BUF_LEN; ++i) {
			mfc_get((void*)data[0], (uint32_t)m.data_ptr + i * len * sizeof(int), len * sizeof(int), tag_id[0], 0, 0);
			wait_tag(tag_id[0]);

			if (1 == m.vectorial) {
		        	if (1 == m.op)
                        		encrypt_vector(data[0], key, len);
	                	else
                        		decrypt_vector(data[0], key, len);

			} else {
				if (1 == m.op)
					encrypt(data[0], key, len);
				else
					decrypt(data[0], key, len);
			}
			mfc_put((void*)data[0], (uint32_t)m.data_ptr + i * len * sizeof(int), len * sizeof(int), tag_id[0], 0, 0);
			wait_tag(tag_id[0]);
		}
	}


	mfc_tag_release(tag_id[0]);
	mfc_tag_release(tag_id[1]);
	return 0;
}

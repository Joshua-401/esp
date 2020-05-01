#include "../inc/espacc_config.h"
#include "../inc/espacc.h"
#include "hls_stream.h"
#include "hls_math.h"
#include <cstring>

inline double MAX(double A, double B) {
  return A > B ? A : B;
}

void load(word_t _inbuff[SIZE_IN_CHUNK_DATA], dma_word_t *in1,
          /* <<--compute-params-->> */
	 const unsigned ALEN,
	 const unsigned BLEN,
	  dma_info_t *load_ctrl, int chunk, int batch)
{
load_data:

    const unsigned length = round_up(ALEN + BLEN, VALUES_PER_WORD) / 1;
    const unsigned index = length * (batch * 1 + chunk);

    unsigned dma_length = (ALEN + BLEN) / VALUES_PER_WORD;
    // unsigned dma_length = length / VALUES_PER_WORD;
    unsigned dma_index = index / VALUES_PER_WORD;

    load_ctrl[0].index = 0;
    // load_ctrl[0].index = dma_index;
    load_ctrl[0].length = dma_length;
    load_ctrl[0].size = SIZE_WORD_T;

    for (unsigned i = 0; i < dma_length; i++) {
    load_label0:for(unsigned j = 0; j < VALUES_PER_WORD; j++) {
	    _inbuff[i * VALUES_PER_WORD + j] = in1[dma_index + i].word[j];
    	}
    }
} 

void store(word_t _outbuff[SIZE_OUT_CHUNK_DATA], dma_word_t *out,
          /* <<--compute-params-->> */
	 const unsigned ALEN,
	 const unsigned BLEN,
	   dma_info_t *store_ctrl, int chunk, int batch)
{
store_data:

    const unsigned length = round_up(2 * (ALEN + BLEN), VALUES_PER_WORD) / 1;
    const unsigned store_offset = round_up(ALEN + BLEN, VALUES_PER_WORD) * 1;
    const unsigned out_offset = store_offset;
    const unsigned index = out_offset + length * (batch * 1 + chunk);

    unsigned dma_length = length / VALUES_PER_WORD;
    unsigned dma_index = index / VALUES_PER_WORD;

    store_ctrl[0].index = dma_index;
    store_ctrl[0].length = dma_length;
    store_ctrl[0].size = SIZE_WORD_T;

    for (unsigned i = 0; i < dma_length; i++) {
    store_label1:for(unsigned j = 0; j < VALUES_PER_WORD; j++) {
	    out[dma_index + i].word[j] = _outbuff[i * VALUES_PER_WORD + j];
	}
    }
}
     

void compute(word_t _inbuff[SIZE_IN_CHUNK_DATA],
             /* <<--compute-params-->> */
	 const unsigned ALEN,
	 const unsigned BLEN,
             word_t _outbuff[SIZE_OUT_CHUNK_DATA])
{
    word_t *SEQA = &_inbuff[0];
    word_t *SEQB = &_inbuff[ALEN];
    word_t *alignedA = &_outbuff[0];
    word_t *alignedB = &_outbuff[ALEN + BLEN];

    // TODO implement compute functionality
    const int MATCH_SCORE = 1;
    const int MISMATCH_SCORE = -1;
    const int GAP_SCORE = -1;
    const int ROWA = 129;
    const int COLB = 129;

    const char ALIGN = '\\';
    const char SKIPA = '^';
    const char SKIPB = '<';

    int M[ROWA * COLB];
    char ptr[ROWA * COLB];
    int score, up_left, up, left, max;
    int row, row_up, r;
    int a_idx, b_idx;
    int a_str_idx, b_str_idx;

    init_row: for(a_idx=0; a_idx<(ALEN+1); a_idx++){
        M[a_idx] = a_idx * GAP_SCORE;
    }
    init_col: for(b_idx=0; b_idx<(BLEN+1); b_idx++){
        M[b_idx*ROWA] = b_idx * GAP_SCORE;
    }

    // Matrix filling loop
    fill_out: for(b_idx=1; b_idx<(BLEN+1); b_idx++){
        fill_in: for(a_idx=1; a_idx<(ALEN+1); a_idx++){
            if(SEQA[a_idx-1] == SEQB[b_idx-1]){
                score = MATCH_SCORE;
            } else {
                score = MISMATCH_SCORE;
            }

	    row_up = (b_idx-1)*(ALEN+1);
            row = (b_idx)*(ALEN+1);

            up_left = M[row_up + (a_idx-1)] + score;
            up      = M[row_up + (a_idx  )] + GAP_SCORE;
            left    = M[row    + (a_idx-1)] + GAP_SCORE;

            max = MAX(up_left, MAX(up, left));

            M[row + a_idx] = max;
            if(max == left){
                ptr[row + a_idx] = SKIPB;
            } else if(max == up){
                ptr[row + a_idx] = SKIPA;
            } else{
                ptr[row + a_idx] = ALIGN;
            }
        }
    }

    // TraceBack (n.b. aligned sequences are backwards to avoid string appending)
    a_idx = ALEN;
    b_idx = BLEN;
    a_str_idx = 0;
    b_str_idx = 0;

    trace: while(a_idx>0 || b_idx>0) {
        r = b_idx*(ALEN+1);
        if (ptr[r + a_idx] == ALIGN){
            alignedA[a_str_idx++] = SEQA[a_idx-1];
            alignedB[b_str_idx++] = SEQB[b_idx-1];
            a_idx--;
            b_idx--;
        }
        else if (ptr[r + a_idx] == SKIPB){
            alignedA[a_str_idx++] = SEQA[a_idx-1];
            alignedB[b_str_idx++] = '-';
            a_idx--;
        }
        else{
            alignedA[a_str_idx++] = '-';
            alignedB[b_str_idx++] = SEQB[b_idx-1];
            b_idx--;
        }
    }

    // Pad the result
    pad_a: for( ; a_str_idx<ALEN+BLEN; a_str_idx++ ) {
      alignedA[a_str_idx] = '_';
    }
    pad_b: for( ; b_str_idx<ALEN+BLEN; b_str_idx++ ) {
      alignedB[b_str_idx] = '_';
    }

}


void top(dma_word_t *out, dma_word_t *in1,
         /* <<--params-->> */
	 const unsigned conf_info_ALEN,
	 const unsigned conf_info_BLEN,
	 dma_info_t *load_ctrl, dma_info_t *store_ctrl)
{

    /* <<--local-params-->> */
	 const unsigned ALEN = conf_info_ALEN;
	 const unsigned BLEN = conf_info_BLEN;

    // Batching
batching:
    for (unsigned b = 0; b < 1; b++)
    {
        // Chunking
    go:
        for (int c = 0; c < 1; c++)
        {
            word_t _inbuff[SIZE_IN_CHUNK_DATA];
            word_t _outbuff[SIZE_OUT_CHUNK_DATA];

            load(_inbuff, in1,
                 /* <<--args-->> */
	 	 ALEN,
	 	 BLEN,
                 load_ctrl, c, b);
            compute(_inbuff,
                    /* <<--args-->> */
	 	 ALEN,
	 	 BLEN,
                    _outbuff);
            store(_outbuff, out,
                  /* <<--args-->> */
	 	 ALEN,
	 	 BLEN,
                  store_ctrl, c, b);
        }
    }
}

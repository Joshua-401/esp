#include "../inc/espacc_config.h"
#include "../inc/espacc.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {

    printf("****start*****\n");

    /* <<--params-->> */
	 const unsigned ALEN = 128;
	 const unsigned BLEN = 128;

    uint32_t in_words_adj;
    uint32_t in_words_adj_A;
    uint32_t in_words_adj_B;
    uint32_t out_words_adj;
    uint32_t out_words_adj_A;
    uint32_t out_words_adj_B;
    uint32_t in_size;
    uint32_t in_size_A;
    uint32_t in_size_B;
    uint32_t out_size;
    uint32_t out_size_A;
    uint32_t out_size_B;
    uint32_t dma_in_size;
    uint32_t dma_out_size;
    uint32_t dma_size;


    in_words_adj = round_up(ALEN + BLEN, VALUES_PER_WORD);
    in_words_adj_A = round_up(ALEN, VALUES_PER_WORD);
    in_words_adj_B = round_up(BLEN, VALUES_PER_WORD);
    out_words_adj = round_up(2 * (ALEN + BLEN), VALUES_PER_WORD);
    out_words_adj_A = round_up(ALEN + BLEN, VALUES_PER_WORD);
    out_words_adj_B = round_up(ALEN + BLEN, VALUES_PER_WORD);
    in_size = in_words_adj * (1);
    in_size_A = in_words_adj_A * (1);
    in_size_B = in_words_adj_B * (1);
    out_size = out_words_adj * (1);
    out_size_A = out_words_adj_A * (1);
    out_size_B = out_words_adj_B * (1);


    dma_in_size = in_size / VALUES_PER_WORD;
    dma_out_size = out_size / VALUES_PER_WORD;
    dma_size = dma_in_size + dma_out_size;


    dma_word_t *mem=(dma_word_t*) malloc(dma_size * sizeof(dma_word_t));
    word_t *inbuff=(word_t*) malloc(in_size * sizeof(word_t));
    word_t *inbuffA=(word_t*) malloc(in_size_A * sizeof(word_t));
    word_t *inbuffB=(word_t*) malloc(in_size_B * sizeof(word_t));
    word_t *outbuff=(word_t*) malloc(out_size * sizeof(word_t));
    word_t *outbuff_gold= (word_t*) malloc(out_size * sizeof(word_t));
    word_t *outbuffA=(word_t*) malloc(out_size_A * sizeof(word_t));
    word_t *outbuffB=(word_t*) malloc(out_size_B * sizeof(word_t));
    dma_info_t *load = (dma_info_t*) malloc(sizeof(dma_info_t));
    dma_info_t *store = (dma_info_t*) malloc(sizeof(dma_info_t));


    // Prepare input data
    //FILE *fileA = NULL;

    //fileA = fopen("input.data", "r");
    //if (fileA == NULL) {
    //  std::cout << "fileA not found" << std::endl;
    //}

    //char val_char = 0;
    // fscanf(fileA, "%c", inbuff);

    //fclose(fileA); 


    FILE *fileA = NULL;

    if((fileA = fopen("/home/esp2020/js5506/prj-spring2020-js5506/esp/accelerators/vivado_hls/nw2/syn/input.data", "r")) == (FILE*) NULL)
      {
        printf("[ERROR] Could not open input.data\n");
        exit(1);
      }

    int i = 0;
    char val_char = 0;
    fscanf(fileA, "%c", &val_char);
    while(!feof(fileA))
      {
        inbuff[i++] = val_char;
        fscanf(fileA, "%c", &val_char);
      }
    fclose(fileA);

    //std::string sequenceA = "tcgacgaaataggatgacagcacgttctcgtattagagggccgcggtacaaaccaaatgctgcggcgtacagggcacggggcgctgttcgggagatcgggggaatcgtggcgtgggtgattcgccggc";
    //std::string sequenceB = "ttcgagggcgcgtgtcgcggtccatcgacatgcccggtcggtgggacgtgggcgcctgatatagaggaatgcgattggaaggtcggacgggtcggcgagttgggcccggtgaatctgccatggtcgat";

    //for(int i = 0; i < ALEN; i++)
      //seqA[i] = sequenceA[i];
    
   //for(int i = 0; i < BLEN; i++)
      //seqB[i] = sequenceB[i];

    for(unsigned i = 0; i < 1; i++)
        for(unsigned j = 0; j < ALEN + BLEN; j++)
            inbuff[i * in_words_adj + j] = (word_t) j;

    //for(unsigned i = 0; i < dma_in_size; i++)
    //	for(unsigned k = 0; k < VALUES_PER_WORD; k++)
    //	    mem[i].word[k] = inbuff[i * VALUES_PER_WORD + k];

    for(unsigned i = 0; i < ALEN; i++)
    	for(unsigned k = 0; k < VALUES_PER_WORD; k++){
                    inbuffA[i] = inbuff[i];
    	    mem[i].word[k] = inbuffA[i * VALUES_PER_WORD + k];
     }

     for(unsigned i = 0; i < BLEN; i++)
    	for(unsigned k = 0; k < VALUES_PER_WORD; k++){
                    inbuffB[i] = inbuff[i + ALEN];
    	    mem[i + ALEN].word[k] = inbuffB[i * VALUES_PER_WORD + k]; 
      }



    // Set golden output

    //std::string checkA = "cggccgcttag-tgggtgcggtgctaagggggctagagggcttg-tc-gcggggcacgggacatgcg--gcg-t--cgtaaaccaaacat-g-gcgccgggag-attatgctcttgcacg-acag-ta----g-gat-aaagc---agc-t_________________________________________________________________________________________________________";
    //std::string checkB = "--------tagct-ggtaccgt-ctaa-gtggc--ccggg-ttgagcggctgggca--gg-c-tg-gaag-gttagcgt-aaggagatatagtccg-cgggtgcagggtg-gctggcccgtacagctacctggcgctgtgcgcgggagctt_________________________________________________________________________________________________________";

    //for(int i = 0; i < ALEN + BLEN; i++){
     //alignedA[i] = checkA[i];
     //alignedB[i] = checkB[i];
    //}   
    FILE *fileB = NULL;

    if((fileB = fopen("/home/esp2020/js5506/prj-spring2020-js5506/esp/accelerators/vivado_hls/nw2/syn/check.data", "r")) == (FILE*) NULL)
      {
        printf("[ERROR] Could not open check.data\n");
        exit(1);
      }

    int j = 0;
    fscanf(fileB, "%c", &val_char);
    while(!feof(fileB))
      {
        outbuff_gold[j++] = val_char;
        fscanf(fileB, "%c", &val_char);
      }

    fclose(fileB);  

    for(unsigned i = 0; i < 1; i++)
        for(unsigned j = 0; j < 2 * (ALEN + BLEN); j++){
            outbuff_gold[i * out_words_adj + j] = (word_t) j;
    }
   

    // Call the TOP function
    top(mem, mem,
        /* <<--args-->> */
	 	 ALEN,
	 	 BLEN,
        load, store);

    // Validate
    uint32_t out_offset = dma_in_size;
    for(unsigned i = 0; i < ALEN + BLEN; i++)
	for(unsigned k = 0; k < VALUES_PER_WORD; k++){
                    outbuffA[i] = outbuff[i];
	    outbuffA[i * VALUES_PER_WORD + k] = mem[out_offset + i].word[k];
    }

    for(unsigned i = 0; i < ALEN + BLEN; i++)
	for(unsigned k = 0; k < VALUES_PER_WORD; k++){
                    outbuffB[i] = outbuff[i + ALEN + BLEN];
	    outbuffB[i * VALUES_PER_WORD + k] = mem[out_offset + i + ALEN +BLEN].word[k];
     }

    int errors = 0;

    for(unsigned i = 0; i < 1; i++)
        for(unsigned j = 0; j < 2 * (ALEN + BLEN); j++)
	    if (outbuff[i * out_words_adj + j] != outbuff_gold[i * out_words_adj + j])
		errors++;

    if (errors)
	std::cout << "Test FAILED with " << errors << " errors." << std::endl;
    else
	std::cout << "Test PASSED." << std::endl;

    // Free memory



    free(mem);
    free(inbuff);
    free(inbuffA);
    free(inbuffB);
    free(outbuff);
    free(outbuff_gold);
    free(outbuffA);
    free(outbuffB);
    free(load);
    free(store);

    return 0;
}

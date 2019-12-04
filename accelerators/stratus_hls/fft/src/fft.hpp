// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#ifndef __FFT_HPP__
#define __FFT_HPP__

#include "fpdata.hpp"
#include "fft_conf_info.hpp"
#include "fft_debug_info.hpp"

#include "esp_templates.hpp"

#include "fft_directives.hpp"

#define __round_mask(x, y) ((y)-1)
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)
/* <<--defines-->> */
#define DATA_WIDTH 64
#define DMA_SIZE SIZE_DWORD
#define PLM_OUT_WORD 32768
#define PLM_IN_WORD 32768

class fft : public esp_accelerator_3P<DMA_WIDTH>
{
public:
    // Constructor
    SC_HAS_PROCESS(fft);
    fft(const sc_module_name& name)
    : esp_accelerator_3P<DMA_WIDTH>(name)
        , cfg("config")
    {
        // Signal binding
        cfg.bind_with(*this);

        // Map arrays to memories
        /* <<--plm-bind-->> */
        HLS_MAP_plm(B0, PLM_OUT_NAME);
        HLS_MAP_plm(C, PLM_OUT_NAME);
        HLS_MAP_plm(A0, PLM_IN_NAME);
    }

    // Processes

    // Load the input data
    void load_input();

    // Computation
    void compute_kernel();

    // Store the output data
    void store_output();

    // Configure fft
    esp_config_proc cfg;

    // Functions

    // Private local memories
    sc_dt::sc_int<DATA_WIDTH> A0[PLM_IN_WORD];
    sc_dt::sc_int<DATA_WIDTH> C[PLM_OUT_WORD];
    sc_dt::sc_int<DATA_WIDTH> B0[PLM_OUT_WORD];

};


#endif /* __FFT_HPP__ */

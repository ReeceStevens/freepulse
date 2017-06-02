#ifndef __spo2_h__
#define __spo2_h__


#include "AFE4400.h"

class Sampler {
private:
    CircleBuffer<int32_t> red_signal;
    CircleBuffer<int32_t> ir_signal;
    Pin_Num cs, rst, adc_rdy, adc_pdn;

    Sampler(

    /*
     * Sampling Data Filter
     *
     */
    double sampling_sos_filter[5][6] = {
        {1, 2, 1, 1, -1.9461, 0.9615},
        {1, 2, 1, 1, -1.8774, 0.8923},
        {1, 2, 1, 1, -1.8227, 0.8372},
        {1, 2, 1, 1, -1.7849, 0.7991},
        {1, 2, 1, 1, -1.7657, 0.7797},
    };

    double sampling_gain[5] = {0.0039, 0.0037, 0.0036, 0.0035, 0.0035};

    double red_xs[5][3] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };
    double red_ws[5][3] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };

    double filter_red(int32_t x) {
        red_xs[0][0] = (double) x;
        double y;
        for (int i = 0; i < 5; i ++){
            red_xs[i][0] *= sampling_gain[i];
            // Apply SOS
            red_ws[i][0] = red_xs[i][0] - sampling_sos_filter[i][4]*red_ws[i][1] - sampling_sos_filter[i][5]*red_ws[i][2];
            y = sampling_sos_filter[i][0]*red_ws[i][0] + sampling_sos_filter[i][1]*red_ws[i][1] + sampling_sos_filter[i][2]*red_ws[i][2]; 
            // Shift coefficients
            red_xs[i][2] = red_xs[i][1];
            red_xs[i][1] = red_xs[i][0];
            red_ws[i][2] = red_ws[i][1];
            red_ws[i][1] = red_ws[i][0];
            // Carry over to next section
            if (i != 4) {
                red_xs[i+1][0] = y;    
            }
        }
        return y;
    }

    double ir_xs[5][3] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };
    double ir_ws[5][3] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };

    double filter_ir(int32_t x) {
        ir_xs[0][0] = (double) x;
        double y;
        for (int i = 0; i < 5; i ++){
            ir_xs[i][0] *= sampling_gain[i];
            // Apply SOS
            ir_ws[i][0] = ir_xs[i][0] - sampling_sos_filter[i][4]*ir_ws[i][1] - sampling_sos_filter[i][5]*ir_ws[i][2];
            y = sampling_sos_filter[i][0]*ir_ws[i][0] + sampling_sos_filter[i][1]*ir_ws[i][1] + sampling_sos_filter[i][2]*ir_ws[i][2]; 
            // Shift coefficients
            ir_xs[i][2] = ir_xs[i][1];
            ir_xs[i][1] = ir_xs[i][0];
            ir_ws[i][2] = ir_ws[i][1];
            ir_ws[i][1] = ir_ws[i][0];
            // Carry over to next section
            if (i != 4) {
                ir_xs[i+1][0] = y;    
            }
        }
        return y;
    }
};

#endif

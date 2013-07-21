#include "YMatePair.h"

#include <iostream>

YMatePair::orientation_flag flag_from_maq_tag(bam1_t const* b) {
    YMatePair::orientation_flag readflag = YMatePair::NF;

    // already know this should be paired so check if mate
    // information is available otherwise exit
    if(b->core.mtid < 0 ) {
        uint8_t* result = bam_aux_get(b, "MF");
        if(result) {
            int32_t flag = bam_aux2i(result);
            switch(flag) {
                case 18:
                    readflag = YMatePair::FR;
                    break;
                case 64:
                    readflag = YMatePair::PM;
                    break;
                case 192:
                    readflag = YMatePair::PM;
                    break;
                case 1:
                    readflag = YMatePair::FF;
                    break;
                case 4:
                    readflag = YMatePair::RF;
                    break;
                case 8:
                    readflag = YMatePair::RR;
                    break;
                case 32:
                    readflag = YMatePair::CT;
                    break;
            }
        }
        else {
            std::cerr << "Did not have embedded mate pair"
                "information and could not find MAQ flag for read"
                "coloring. Unable to color reads.\n";
        }
    }
    return readflag;
}

YMatePair::orientation_flag flag_from_pair_info(bam1_t const* b) {
    YMatePair::orientation_flag readflag = YMatePair::NF;

    if(b->core.tid != b->core.mtid) {
        readflag = YMatePair::CT;
    }
    else {
        unsigned int strand_flags =  b->core.flag & (BAM_FREVERSE | BAM_FMREVERSE);
        // This may be invalid if mpos and pos are the same ever.
        // When this happens currently it gets set to NF.
        if(b->core.pos < b->core.mpos) {
            if( strand_flags == (BAM_FREVERSE | BAM_FMREVERSE)) {
                readflag = YMatePair::RR;
            }
            else if(strand_flags == BAM_FREVERSE) {
                readflag = YMatePair::RF;
            }
            else if(strand_flags == BAM_FMREVERSE) {
                readflag = YMatePair::FR;
            }
            else {
                readflag = YMatePair::FF;
            }
        }
        if(b->core.pos > b->core.mpos) {
            if( strand_flags == (BAM_FREVERSE | BAM_FMREVERSE)) {
                readflag = YMatePair::RR;
            }
            else if(strand_flags == BAM_FREVERSE) {
                readflag = YMatePair::FR;
            }
            else if(strand_flags == BAM_FMREVERSE) {
                readflag = YMatePair::RF;
            }
            else {
                readflag = YMatePair::FF;
            }
        }
    }

    return readflag;
}


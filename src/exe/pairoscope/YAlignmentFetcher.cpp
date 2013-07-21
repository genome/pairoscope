#include "YAlignmentFetcher.h"

#include "YBDConfig.h"

#include <sam.h>

#include <algorithm>
#include <cerrno>
#include <iostream>
#include <stdexcept>

extern "C" {
    // These are in bam_aux.c (from samtools), but not in a header.
    int32_t bam_get_tid(const bam_header_t *header, const char *seq_name);
    void bam_init_header_hash(bam_header_t *header);
}

extern "C" {
    // Callback helper for samtools pileup API.
    static int pileup_func(
            uint32_t tid,
            uint32_t pos,
            int n,
            const bam_pileup1_t *pl,
            void *data
            )
    {
        YAlignmentFetcher::depth_buffer* obj(
            reinterpret_cast<YAlignmentFetcher::depth_buffer*>(data)
            );

        return obj->pileupCallback(tid, pos, n, pl);
    }
}

YAlignmentFetcher::YAlignmentFetcher(
        char const* filename,
        std::set<int> const& flags,
        int minQual,
        int buffer,
        bool includeNormal,
        int lower_bound,
        int upper_bound,
        int minimum_isize,
        std::vector<YMatePair*>* mates,
        hash_map_char<YMatePair*>* unpaired_reads,
        YBDConfig *config,
        int stddev
        )
    : filename(filename)
    , flags(flags)
    , minQual(minQual)
    , buffer(buffer)
    , includeNormal(includeNormal)
    , lower_bound(lower_bound)
    , upper_bound(upper_bound)
    , minimum_isize(minimum_isize)
    , mates(mates)
    , unpaired_reads(unpaired_reads)
    , config(config)
    , stddev(stddev)
    , in(samopen(filename, "rb", 0))
{
    if (in == 0) {
        std::stringstream ss;
        ss << "Failed to open BAM file " << filename;
        throw std::runtime_error(ss.str());
    }

    idx = bam_index_load(filename); // load BAM index
    if (idx == 0) {
        std::stringstream ss;
        ss << "Failed to load BAM index for " << filename;
        throw std::runtime_error(ss.str());
    }

    if(in->header) {
        bam_init_header_hash(in->header);
    }
    else {
        std::stringstream ss;
        ss << "Header required in bam file " << filename;
        throw std::runtime_error(ss.str());
    }
}

YAlignmentFetcher::~YAlignmentFetcher() {
    bam_index_destroy(idx);
    samclose(in);
}

int YAlignmentFetcher::pushRead(bam1_t const* b) {
    if(b->core.qual >= minQual && !(b->core.flag & BAM_DEF_MASK)) {
        YMatePair::orientation_flag readflag = YMatePair::NF; //no flag
        if(b->core.flag & BAM_FPAIRED) {
            if(b->core.flag & BAM_FMUNMAP) {
                readflag = YMatePair::PM;  // partial match
            }
            else {
                // already know this should be paired so check if mate
                // information is available. if not, we can try the MAQ
                // MF flag...
                if(b->core.mtid < 0 ) {
                    readflag = flag_from_maq_tag(b);
                }
                else {
                    readflag = flag_from_pair_info(b);
                }

                // check the size to determine if it hits our insertion/deletion
                // cutoffs, // only flagging reads with the default Illumina
                // library orientation ie "normal" reads.
                // note that this requires proper filling of isize and would not
                // work for maq.
                int abs_size = abs(b->core.isize);
                if(config && readflag == YMatePair::FR) {
                    //grab readgroup
                    if(uint8_t* tmp = bam_aux_get(b, "RG")) {
                        std::string readgroup(bam_aux2Z(tmp));
                        double rg_stddev = config->stddev_for_readgroup(readgroup);
                        double rg_mean = config->mean_for_readgroup(readgroup);
                        double calculated_lower = rg_mean - stddev * rg_stddev;
                        if(calculated_lower < 0) {
                            calculated_lower = 0;
                        }
                        double calculated_upper = rg_mean + stddev * rg_stddev;
                        if(abs_size > calculated_upper) {
                            readflag = YMatePair::DL;
                        } else if(abs_size < calculated_lower) {
                            readflag = YMatePair::IN;
                        }
                    }
                }
                if(readflag == YMatePair::FR) {
                    if(lower_bound != -1 && abs_size < lower_bound) {
                        //assume insertion
                        readflag = YMatePair::IN;
                    }
                    else if(upper_bound != -1 && abs_size > upper_bound) {
                        readflag = YMatePair::DL;
                    }
                }

                //enforce the minimum size by marking such reads as NF
                if(abs_size < minimum_isize && readflag != YMatePair::CT) {
                    readflag = YMatePair::NF;
                }
            }

        }

        if(wantReadFlag(readflag)) {
            //abnormal read
            char* name = bam1_qname(b);
            YMatePair* mate;
            if(unpaired_reads->find(name,&mate)) {
                //then we've already found this read
                //remove from the hash
                unpaired_reads->erase(name);

                mate->rightReadPosition = b->core.pos + 1; //bam positions are 0 based
                char* tidName = in->header->target_name[b->core.tid];
                char* refName = new char[strlen(tidName) + 1];
                strcpy(refName, tidName);
                mate->rightRefName = refName;

                if(readflag != mate->orientation) {
                    std::cerr << "Non-matching mate orientation.\n";
                }
            }
            else {
                mate = new YMatePair;
                mate->orientation = readflag;
                mate->leftReadPosition = b->core.pos + 1;

                mate->readLength = b->core.l_qseq;
                mate->bestMappingQuality = b->core.qual;
                mate->rightReadPosition = 0;
                char* tidName = in->header->target_name[b->core.tid];
                char* refName = new char[strlen(tidName) + 1];
                strcpy(refName, tidName);
                mate->leftRefName = refName;
                unpaired_reads->insert(name,mate);
                mates->push_back(mate);
            }
        }
    }

    return 0;
}

bool YAlignmentFetcher::wantReadFlag(YMatePair::orientation_flag flag) const {
    return includeNormal
            || (!(flag == YMatePair::NF) && !(flag == YMatePair::FR)
                && (flags.empty() || flags.find(flag) != flags.end() ));
}

// callback for bam_plbuf_init()
void YAlignmentFetcher::fetchBAMAlignments(
        char const* refName,
        unsigned start,
        unsigned end,
        depth_buffer* depth
        )
{
    start = start - 1 - buffer;
    end = end + buffer;

    int tid = bam_get_tid(in->header, refName);
    if(tid == -1) {
        std::stringstream ss;
        ss << "Sequence name '" << refName << "' not found in BAM file";
        throw std::runtime_error(ss.str());
    }

    bam_plbuf_t* pileup_buffer = bam_plbuf_init(pileup_func, depth);
    depth->setRegion(refName, start, end);

    bam_iter_t iter = bam_iter_query(idx, tid, start, end);
    bam1_t* b = bam_init1();
    while (bam_iter_read(in->x.bam, iter, b) > 0) {
        pushRead(b);
        bam_plbuf_push(b, pileup_buffer);
    }
    bam_iter_destroy(iter);
    bam_destroy1(b);

    bam_plbuf_push(0, pileup_buffer); // finalize pileup
    bam_plbuf_destroy(pileup_buffer);
}

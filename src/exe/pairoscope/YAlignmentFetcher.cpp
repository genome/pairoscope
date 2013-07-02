/*----------------------------------
  $Author$ 
  $Date$
  $Revision$
  $URL$
  -----------------------------------

  ENTER DESCRIPTION HERE

  ----------------------------------*/

#include "YAlignmentFetcher.h"
#include <errno.h>
#include <stdio.h>
#include <algorithm>
#include "sam.h"

//Probably going to need to define some extern "C" functions and structs here to pass to bam as callbacks (BLEAH)

//C struct to store crapola for bam callbacks. Probably a more intelligent way to do this
typedef struct {
    int tid;
    int min_qual;
    int beg,end;
    int include_normal;
    samfile_t *in;
    std::vector<YMatePair*> *mates;
    std::vector<int> *depth;
    hash_map_char<YMatePair*> *unpaired_reads;    
    std::set<int> *flags;
    int lower_bound;
    int upper_bound;
    int minimum_size;
    YBDConfig *config;
    bam_plbuf_t *buf;
} pileup_data_t;

//this is in bam_aux.c but not in a header 
extern "C" { 
    int32_t bam_get_tid(const bam_header_t *header, const char *seq_name);
    void bam_init_header_hash(bam_header_t *header);
}

// callback for bam_fetch()
// This will retrieve MF flag, add the read to the hash, and add read to pileup buffer
extern "C" {
    static int fetch_func(const bam1_t *b, void *data);
}
static int fetch_func(const bam1_t *b, void *data)
{
    pileup_data_t *d = (pileup_data_t*)data;
    if(b->core.qual >= d->min_qual && !(b->core.flag & BAM_DEF_MASK)) {

        bam_plbuf_push(b, d->buf);  //push into pileup for depth calculations
        YMatePair::orientation_flag readflag = YMatePair::NF; //no flag
        if(b->core.flag & BAM_FPAIRED) {
            if(b->core.flag & BAM_FMUNMAP) {
                readflag = YMatePair::PM;  //partial match     
            }
            else {
                //already know this should be paired so check if mate information is available otherwise exit
                if(b->core.mtid < 0 ) {
                    uint8_t* result = bam_aux_get(b, "MF");
                    if(result) {
                        int32_t maq_flag = bam_aux2i(result);
                        switch(maq_flag) {
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
                        fprintf(stderr, "Did not have embedded mate pair information and could not find MAQ flag for read coloring. Unable to color reads.\n");
                    }
                }
                else {
                    if(b->core.tid != b->core.mtid) {
                        readflag = YMatePair::CT;
                    }
                    else {
                        unsigned int strand_flags =  b->core.flag & (BAM_FREVERSE | BAM_FMREVERSE);
                        //This may be invalid if mpos and pos are the same ever. When this happens currently it gets set to NF
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
                }
                //check the size to determine if it hits our insertion/deletion cutoffs
                //only flagging reads with the default Illumina library orientatio ie "normal" reads
                //note that this requires proper filling of isize and would not work for maq
                int abs_size = abs(b->core.isize);
                if(d->config) {
                    //grab readgroup
                    //compare to Config stats via some sort of sd
                    //set flags

                }
                if(readflag == YMatePair::FR) { 
                    if(abs_size < d->lower_bound) {
                        //assume insertion
                        readflag = YMatePair::IN;
                    }
                    else if(abs_size > d->upper_bound) {
                        readflag = YMatePair::DL;
                    }
                }

                //enforce the whole minimum size thing by marking such reads as NF
                if(abs_size < d->minimum_size && readflag != YMatePair::CT) {
                    readflag = YMatePair::NF;
                }
            }
                
        }
        //fprintf(stderr,"MF = %i\n",maq_flag);
        if(d->include_normal || (!(b->core.flag & BAM_FPROPER_PAIR) && !(readflag == YMatePair::NF) && (d->flags->empty() || d->flags->find(readflag) != d->flags->end() ))  ) {
            //abnormal read
            char *name = bam1_qname(b);
            YMatePair* mate;
            if(d->unpaired_reads->find(name,&mate)) {
                //then we've already found this read
                //fprintf(stdout,"Found %#X\n",mate);
                //remove from the hash
                d->unpaired_reads->erase(name);

                mate->rightReadPosition = b->core.pos + 1; //bam positions are 0 based
                char *tidName = d->in->header->target_name[b->core.tid];
                char *refName = new char[strlen(tidName) + 1];
                strcpy(refName, tidName);
                //fprintf(stderr, "Read Ref name = %s\n", refName);
                mate->rightRefName = refName;

                if(readflag != mate->orientation) {
                    fprintf(stderr, "Non-matching mate orientation.\n");
                }
            }
            else {
                mate = new YMatePair;
                mate->orientation = readflag;
                mate->leftReadPosition = b->core.pos + 1;

                mate->readLength = b->core.l_qseq;
                mate->bestMappingQuality = b->core.qual;
                mate->rightReadPosition = 0;
                char *tidName = d->in->header->target_name[b->core.tid];
                char *refName = new char[strlen(tidName) + 1];
                strcpy(refName, tidName);
                mate->leftRefName = refName;
                d->unpaired_reads->insert(name,mate);
                d->mates->push_back(mate);

            }
        }

    }
    return 0;
}

// callback for bam_plbuf_init()
extern "C" {
    static int pileup_func(uint32_t tid, uint32_t pos, int n, const bam_pileup1_t *pl, void *data);
}

static int pileup_func(uint32_t tid, uint32_t pos, int n, const bam_pileup1_t *pl, void *data)
{
    pileup_data_t *tmp = (pileup_data_t*)data;
    int mapq_n = 0;
    if ((int)pos >= tmp->beg && (int)pos < tmp->end) {
        if(tmp->depth->empty()) {
            //zero out the vector for the region
            tmp->depth->assign(tmp->end - tmp->beg,0);
        }
        //loop over the bases
        int i;
        for(i = 0; i < n; ++i) {
            const bam_pileup1_t *base = pl + i;
            if(!base->is_del) {
                //fprintf(stderr,"%s\n",bam1_qname(base->b));
                mapq_n++;                
            }
        }
        //set depth at position
        tmp->depth->at(pos - tmp->beg) = mapq_n;
    }
    return 0;
}

bool YAlignmentFetcher::fetchBAMAlignments(const char* filename, const char *refName, unsigned int start, unsigned int end, std::vector<int> *depth, std::vector<YMatePair*> *mates, hash_map_char<YMatePair*> *unpaired_reads, std::set<int> *flags, int lower_bound, int upper_bound, int minimum_size) {
    //Open the region in the bam file
    //Parse appropriately the MF field and put into slots as needed
    //return!
    pileup_data_t *d = (pileup_data_t*)calloc(1, sizeof(pileup_data_t));

    d->min_qual = minQual;
    d->beg = start-1-buffer;
    d->end = end+buffer;
    d->depth = depth;
    d->mates = mates;
    d->unpaired_reads = unpaired_reads;
    d->flags = flags;
    d->include_normal = this->includeNormal;
    d->lower_bound = lower_bound; //by default mark no reads as insertions
    d->upper_bound = upper_bound;    //max integer value to expect in isize. Could this overflow on some systems? 
    d->minimum_size = minimum_size;
    d->config = config;
    d->in = samopen(filename, "rb", 0);

    if (d->in == 0) {
        fprintf(stderr, "Failed to open BAM file %s\n", filename);
        return 0;
    }
    bam_index_t *idx;
    idx = bam_index_load(filename); // load BAM index
    if (idx == 0) {
        fprintf(stderr, "BAM indexing file is not available.\n");
        return 0;
    }
    if(d->in->header) {
        bam_init_header_hash(d->in->header);
        d->tid = bam_get_tid(d->in->header, refName);
    }
    else {
        fprintf(stderr, "Header required in bam file %s\n", filename);
        return 0;
    }
    if(d->tid == -1) {
        fprintf(stderr, "Reference id %s not found in BAM file\n",refName);
        return 0;
    }
    d->buf = bam_plbuf_init(pileup_func, d); // initialize pileup
    bam_fetch(d->in->x.bam, idx, d->tid, d->beg, d->end, d, fetch_func);
    bam_plbuf_push(0, d->buf); // finalize pileup
    bam_index_destroy(idx);
    bam_plbuf_destroy(d->buf);    
    samclose(d->in);
    free(d);
    return 1;
}

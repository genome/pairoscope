/*----------------------------------
  $Author$ 
  $Date$
  $Revision$
  $URL$
  -----------------------------------

  ENTER DESCRIPTION HERE

  ----------------------------------*/

#include "sam.h"
#include "YTranscriptFetcher.h"

//this is in bam_aux.c but not in a header (bad Heng Li)
extern "C" { 
        int32_t bam_get_tid(const bam_header_t *header, const char *seq_name);
        void bam_init_header_hash(bam_header_t *header);
}

//C struct to store transcript info for bam callbacks.
typedef struct {
    int tid;
    int beg,end;
    samfile_t *in;
    std::vector<YTranscript*> *transcripts;
    hash_map_char<YTranscript*> transcriptNames;    
} fetch_data_t;


// callback for bam_fetch()
// This will retrieve transcript substructures and populate the transcripts appropriately
extern "C" {
    static int fetch_func(const bam1_t *b, void *data);
}

static int fetch_func(const bam1_t *b, void *data)
{
    fetch_data_t *d = (fetch_data_t*) data;

    char *name = bam1_qname(b);
    fprintf(stderr,"%s\n",name);
    //TODO Desparately need some error checking on flag retrieval
    char *status = bam_aux2Z(bam_aux_get(b,"YT"));
    char *source = bam_aux2Z(bam_aux_get(b,"YS"));
    int length = bam_aux2i(bam_aux_get(b,"HI"));
    
    //only let through reviewed genbank transcripts
    if(!(strcmp(source, "genbank") == 0)) {
        return 0;
    }
    
    YTranscript* transcript;
    
    YTranscriptSubStructure structure;
    structure.position = b->core.pos + 1;
    structure.length = b->core.l_qseq;
    structure.ordinal = bam_aux2i(bam_aux_get(b,"HI"));
    
    if(d->transcriptNames.find(name,&transcript)) {
        //then we've already found some part of this transcript
        transcript->orderedStructures.push_back(structure);
    }
    else {
        transcript = new YTranscript;

        char *tidName = d->in->header->target_name[b->core.tid];
        char *refName = new char[ strlen(tidName) + 1];
        strcpy(refName, tidName);

        char *transcriptName = new char[ strlen(name) + 1];
        strcpy(transcriptName, name);

        char *flagGeneName = bam_aux2Z(bam_aux_get(b,"YG"));
        char *geneName = new char[ strlen(flagGeneName) + 1];
        strcpy(geneName, flagGeneName);

        char *statusName = new char[ strlen(status + 1) ];
        strcpy(statusName, status);
        
        transcript->gene = geneName;
        transcript->name = transcriptName;
        transcript->refName = refName;
        transcript->status = statusName;
        transcript->orderedStructures.push_back(structure);
        transcript->strand = b->core.flag & BAM_FREVERSE ? -1 : 1;
        transcript->totalNumberOfStructures = bam_aux2i(bam_aux_get(b, "IH"));
        transcript->length = length;

        d->transcriptNames.insert(name,transcript);
        d->transcripts->push_back(transcript);

    }
    return 0;
}

bool YTranscriptFetcher::fetchBAMTranscripts(const char* filename, const char *refName, unsigned int start, unsigned int end, std::vector<YTranscript*> *transcripts) {
    //Open the region in the bam file

    fetch_data_t data;
    fetch_data_t *d = &data;
    d->beg = start-1-buffer;
    d->end = end+buffer;

    d->transcripts = transcripts;
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
    bam_init_header_hash(d->in->header);
    d->tid = bam_get_tid(d->in->header, refName);
    if(d->tid == -1) {
        fprintf(stderr, "Reference id %s not found in BAM file",refName);
        return 0;
    }
    bam_fetch(d->in->x.bam, idx, d->tid, d->beg, d->end, d, fetch_func);
    bam_index_destroy(idx);
    samclose(d->in);
    return 1;
}

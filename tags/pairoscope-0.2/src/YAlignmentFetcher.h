/*----------------------------------
  $Author$ 
  $Date$
  $Revision$
  $URL$
  -----------------------------------

  Enter Description Here

  ----------------------------------*/
#ifndef YALIGNMENTFETCHER_H
#define YALIGNMENTFETCHER_H

#include <vector>
#include <set>
#include "stdhash.hh"
#include "YMatePair.h"
#include <zlib.h>

//! YAlignmentFetcher is a class for reading mapped reads into a data structure format for use with YGenomeView
/*! Currently only the maq .map file format is supported*/

class YAlignmentFetcher {
    int minQual; //!< minimum alignment quality to report
    int buffer;   //!< buffer with which to pad the requested region
    bool includeNormal; //!< whether or not to include normally mapped paired reads in the return hash.
    
    public:
    //! default constructor
    /*! 
        \return a new object
     */
        YAlignmentFetcher() : minQual(0), buffer(0), includeNormal(false) { return;};
        //! standard constructor
    /*! 
        Currently hard coded for a scanWindow size of 300.
        \return a new object
     */
        YAlignmentFetcher(int minQual, int buffer, bool includeNormal) : minQual(minQual), buffer(buffer), includeNormal(includeNormal) { return;};
        
        //! reads in mate pairs from a BAM file 
        /*! 
        fetchBAMAlignments retrieves all the reads from a given region and stores both the depth in the passed depth vector and paired and unpaired reads in the mates vector. Unpaired reads are hashed by read name in the unpaired_reads hash and this can be passed as an argument to a second call to fetchBAMAlignments to pair up reads mapping across different regions e.g. translocations. fetchBAMAlignments currently ignores fragment reads when calculating both depth and mates. In addition, it filters the reads based on their mapping quality of the pair. Note that for "abnormal" pairs, maq may report different mapping qualities for the each read in the pair which is counterintuitive.
        Currently this still relies on the presence of a MF tag in the BAM entries which would
        be invalid in BAM files produced by other aligners. Also, please note that this function properly handles the loading of reads with gaps in their alignments. This does not happen with fetchAlignments.
        
        \param filename the file containing the reads to grab
        \param refName the reference sequence of the region from which to get reads
        \param start the start of the region on the reference
        \param end the end of the region on the reference
        \param depth vector for storing the read depth of the region
        \param mates vector for storing read pairs in the region
        \param unpaired_reads hash of reads without pairs. It is hashed on the reads names
        \return whether or not the process was successful
        
         */
        bool fetchBAMAlignments(const char* filename, const char *refName, unsigned int start, unsigned int end, std::vector<int> *depth, std::vector<YMatePair*> *mates, hash_map_char<YMatePair*> *unpaired_reads, std::set<int> *flags);
};

#endif

#ifndef YALIGNMENTFETCHER_H
#define YALIGNMENTFETCHER_H

#include "io/DepthArray.hpp"

#include "YMatePair.h"
#include "stdhash.hh"

#include <bam.h>
#include <sam.h>

#include <set>
#include <vector>

class YBDConfig;
class YMatePair;

/// \brief YAlignmentFetcher is a class for reading mapped reads into a data
/// structure for use with YGenomeView.
class YAlignmentFetcher {
public:
    typedef DepthArray<int> depth_buffer;

    /// \brief Constructor
    ///
    /// \param filename the file containing the reads to grab.
    /// \param flags set of orientation flags to display.
    /// \param minQual lower mapping quality cutoff.
    /// \param buffer flanking region size.
    /// \param includeNormal if set, include normally mapped pairs in read hash.
    /// \param lower_bound minimum insert size of a normal read pair. Pairs with
    ///     insert size below this threshold are marked as insertions.
    /// \param upper_bound maximum insert size of a normal read pair. Pairs with
    ///     insert size above this threshold are marked as deletions.
    /// \param minimum_isize minimum insert size of a pair to be displayed.
    ///     Pairs with insert size below this threshold are not displayed.
    /// \param mates vector for storing read pairs in the region
    /// \param unpaired_reads hash of reads without pairs. It is hashed on the reads
    ///     names
    /// \param config optional breakdancer config file to help classify pairs.
    /// \param stddev max # std. dev.s from the mean (reported in the optional
    ///     breakdancer config) used to consider an insert size as "normal".
    ///
    /// \throw std::runtime_error if the bam file and its index cannot be loaded,
    ///     or if refName is not found in the bam header.

    YAlignmentFetcher(
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
            YBDConfig* config = 0,
            int stddev = 0
            );

    /// \brief Destructor
    ~YAlignmentFetcher();

    /// \brief Decide if a read should be included based on its flag
    bool wantReadFlag(YMatePair::orientation_flag flag) const;

    //! Reads in mate pairs from a BAM file.
    /*!
    fetchBAMAlignments retrieves all the reads from a given region and
    stores the depth information in the supplied DepthArray. paired and unpaired
    reads are tracked in the mates and unpaired_reads containers passed to the
    constructor. Unpaired reads are hashed by read name so that the
    unpaired_reads hash can be passed as an argument to a second instance of
    YAlignmentFetcher to pair up reads mapping across different regions e.g.
    translocations. fetchBAMAlignments currently ignores fragment reads when
    calculating both depth and mates. In addition, it filters the reads based on
    their mapping quality of the pair. Note that for "abnormal" pairs, aligners
    may report different mapping qualities for each read in the pair which is
    counterintuitive.

    \param refName the sequence name from which to get reads.
    \param start the start of the region on the reference.
    \param end the end of the region on the reference.
    \param depth vector for storing the read depth of the region

    */

    void fetchBAMAlignments(
        char const* refName,
        unsigned start,
        unsigned end,
        depth_buffer* depth
        );

    /// \brief Called for every read observed
    int pushRead(bam1_t const* b);

private:
    char const* filename;
    std::set<int> const& flags;

    int minQual; ///< minimum alignment quality to report.
    int buffer;  ///< buffer with which to pad the requested region.

    /// include normally mapped paired reads in the return hash if true.
    bool includeNormal;
    int lower_bound;
    int upper_bound;
    int minimum_isize;

    std::vector<YMatePair*>* mates; ///< output: paired reads.
    hash_map_char<YMatePair*>* unpaired_reads; ///< output: unpaired reads.

    YBDConfig *config;
    int stddev;

    samfile_t* in;
    bam_index_t* idx;
};

#endif

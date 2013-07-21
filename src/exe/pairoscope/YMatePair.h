#ifndef YMATEPAIR_H
#define YMATEPAIR_H

#include <bam.h>


//! YMatePair is a simple class to store information about a read pair
/*! YMatePair stores information about reads in a pair. Some of this information is not currently used by yenta, but will likely be populated and used in the future
*/
struct YMatePair {

    //! enum for storing the orientation of the reads relative each other
    enum orientation_flag {
        NF, //!< no flag, default for unfound flag color
        FR, //!< reads are in a forward-reverse orientation
        PM, //!< only one read is matched. A Partial Mapping of the mate pair.
        FF, //!< reads are in forward-forward orientation
        RF, //!< reads are in reverse-forward orientation
        RR, //!< reads are in reverse-reverse orientation
        CT, //!< reads mapped across chromosomes
        DL, //!< reads mapped with distance greater than some cutoff
        IN  //!< reads mapped with distance smaller than some cutoff
    };

    char* libraryName;  //!< library where these reads came from
    unsigned int readLength;    //!< length of the reads
    int leftReadPosition;   //!< position of leftmost read on the reference
    char *leftRefName;          //!< name of the reference (chromosome) that the leftmost read is mapped to
    int rightReadPosition;  //!< position of the rightmost (or second encountered) read on the reference
    char *rightRefName;         //!< name of the reference (chromosome) that the rightmost read is mapped to
    orientation_flag orientation;             //!< flag indicating what orientation the reads are to each other
    int mappingDistance;     //!< distance between the pairs
    int bestMappingQuality;

    //! default constructor for YMatePair
    YMatePair()
        : libraryName(0)
        , readLength(0)
        , leftReadPosition(0)
        , leftRefName(0)
        , rightReadPosition(0)
        , rightRefName(0)
        , orientation(NF)
        , mappingDistance(0)
        , bestMappingQuality(0)
    {
    }

    //! destructor for YMatePair
    ~YMatePair() {
        delete[] leftRefName;
        leftRefName = 0;
        delete[] rightRefName;
        rightRefName = 0;
    }
};

YMatePair::orientation_flag flag_from_maq_tag(bam1_t const* b);
YMatePair::orientation_flag flag_from_pair_info(bam1_t const* b);

#endif

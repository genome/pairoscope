/*----------------------------------
  $Author$ 
  $Date$
  $Revision$
  $URL$
  ----------------------------------*/
  
#ifndef YMATEPAIR_H
#define YMATEPAIR_H

//! YMatePair is a simple class to store information about a read pair
/*! YMatePair stores information about reads in a pair. Some of this information is not currently used by yenta, but will likely be populated and used in the future
*/
class YMatePair {
    public:
    
    //! enum for storing the orientation of the reads relative each other    
    enum orientation_flag {
        FR, //!< reads are in a forward-reverse orientation
        PM, //!< only one read is matched. A Partial Mapping of the mate pair.
        FF, //!< reads are in forward-forward orientation
        RF, //!< reads are in reverse-forward orientation
        RR, //!< reads are in reverse-reverse orientation
        CT, //!< reads mapped across chromosomes 
        DL, //!< reads mapped with distance greater than some cutoff 
        IN, //!< reads mapped with distance smaller than some cutoff 
        NF = 0, //!< no flag, default for unfound flag color
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
    YMatePair() : libraryName(0), readLength(0), leftReadPosition(0), leftRefName(NULL), rightReadPosition(0), rightRefName(NULL), orientation(NF), mappingDistance(0), bestMappingQuality(0) {}
    //! destructor for YMatePair
    ~YMatePair() { 
       if(leftRefName) { delete[] leftRefName; }
       if(rightRefName) { delete[] rightRefName; };
    }
};
    
#endif

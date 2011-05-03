/*----------------------------------
  $Author$ 
  $Date$
  $Revision$
  $URL$
  -----------------------------------

  Enter Description Here

  ----------------------------------*/
#ifndef YTRANSCRIPTSUBSTRUCTURE_H
#define YTRANSCRIPTSUBSTRUCTURE_H

//! YTranscriptSubStructure is a simple class to store information about exons
/*! YTranscriptSubStructure stores information about an exon
 */
class YTranscriptSubStructure {
    public:
        int position;   //!< leftmost coordinate of the exon
        int ordinal;     //!< 0-based number of the exon in transcription orientation
        int length;      //!< length of the exon
        
    
    //! default constructor for YTranscriptSubStructure
        YTranscriptSubStructure() : position(0), ordinal(0), length(0) {}
};
    
#endif

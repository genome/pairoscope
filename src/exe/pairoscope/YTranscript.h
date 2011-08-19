/*----------------------------------
  $Author$ 
  $Date$
  $Revision$
  $URL$
  -----------------------------------

  Enter Description Here

  ----------------------------------*/
#ifndef YTRANSCRIPT_H
#define YTRANSCRIPT_H

#include <vector>
#include "YTranscriptSubStructure.h"

//! YTranscript is a simple class to store information about a transcript
/*! YTranscript stores the exons of a transcript for display. Choosing transcripts will be implemented by the fetcher
 */
class YTranscript {
    public:
        char* gene;  //!< gene the transcript belongs to
        char* name;       //!< transcript name
        char* refName;   //!< chromosome name the transcript is placed on
        char* status;    //!< status of the transcript in question
        int strand;     //!< which strand the transcript is assigned to
        int length;     //!< the length of the coding exons 
        std::vector<YTranscriptSubStructure> orderedStructures; //!< each structure in reference orientation, intended to only store those in a region
        int totalNumberOfStructures;    //!< total number of structures, not just the ones in the region
    
    //! default constructor for YTranscript
        YTranscript() : gene(NULL), name(NULL), refName(NULL), strand(0), length(0), status(NULL) {}
    //! destructor for YTranscript
        ~YTranscript() { 
            if(gene) { delete[] gene; }
            if(name) { delete[] name; };
            if(refName) { delete[] refName; };
            if(status) { delete[] status; };
        }
};
    
#endif

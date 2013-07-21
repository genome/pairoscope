/*----------------------------------
  $Author$
  $Date$
  $Revision$
  $URL$
  -----------------------------------

  Enter Description Here

  ----------------------------------*/
#ifndef YTRANSCRIPTFETCHER_H
#define YTRANSCRIPTFETCHER_H

#include <vector>
#include "stdhash.hh"
#include "YTranscript.h"
#include <zlib.h>
#include <set>
#include <string>

//! YTranscriptFetcher is a class for reading transcript information from a bastardized bam file format

class YTranscriptFetcher {
    int buffer;   //!< buffer with which to pad the requested region

    public:
        //! default constructor
        /*!
           \return a new object
        */
        YTranscriptFetcher() : buffer(0) { return;};
        //! standard constructor
        /*!
            \return a new object
        */
        YTranscriptFetcher(int buffer) : buffer(buffer) { return;};

        bool fetchBAMTranscripts(const char* filename, const char *refName, unsigned int start, unsigned int end, std::vector<YTranscript*> *transcripts, std::set<std::string> *transcriptNames = NULL);
};

#endif

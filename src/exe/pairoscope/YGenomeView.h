/*----------------------------------
  $Author$ 
  $Date$
  $Revision$
  $URL$
  ----------------------------------*/

#ifndef YGENOMEVIEW_H
#define YGENOMEVIEW_H

#include "YPairView.h"
#include "YDepthView.h"
#include "YView.h"
#include <vector>
#include <map>
#include "YMatePair.h"
#include "YGeneView.h"
#include "YColor.h"

//! YGenomeView is a class for drawing read pair information alongside read depth information
/*! This encapsulates drawing multiple regions as tracks. Each region has a depth track and a barcode track. When multiple regions are drawn the depth tracks are segregated at the top of the document and the "barcode" tracks are at the bottom.
 */

class YGenomeView : public YView {
    int geneTracks;
    unsigned int numberTracks;  //!< number of tracks in the view
    std::vector<YMatePair*> *mates; //!< array of mate pairs to draw \sa YAlignmentFetcher
    void layoutTracks();    //!< determines dimensions of the tracks within the View and aligns them to each other
    bool suppressUnpaired; //!< whether or not to skip drawing reads who do not have their mate displayed in the graph. Includes reads with one unmapped
    std::map<YMatePair::orientation_flag,YColor> colorMap; //!< stores color associations to the various flags. Intended to eventually allow configurable colors
    
    public:
        //! standard constructor
        /*! Currently regions have to be added post construction 
            \param cr cairo drawing context
            \param initialFrame YRect of the viewing area in parent coordinates
            \param reads array of mate pairs to be drawn
            \returns a new YGenomeView object
        */
        YGenomeView(cairo_t *cr, YRect initialFrame, std::vector<YMatePair*> *reads, bool isSolid = false)
            : YView(cr, initialFrame), geneTracks(0), numberTracks(0), mates(reads)
        {
            if(isSolid) { setSolidColorScheme(); } else { setIlluminaColorScheme(); } return;
        }
        
        //! adds a region
        /*! Currently regions are identified by their refName and coordinates. This means display of two regions with different alignment files is not supported. 
        \param refName chromosome name
        \param physicalStart start of the region
        \param physicalStop end point of the region
        \param depth array containing depth information for the region
         */
        void addRegion(const char* refName, unsigned int physicalStart, unsigned int physicalStop, std::vector<int> *depth);
        
        
        //! adds a genetrack
        /*! Currently regions are identified by their refName and coordinates. This means display of two regions with different alignment files is not supported. 
        \param refName chromosome name
        \param physicalStart start of the region
        \param physicalStop end point of the region
        \param transcripts array containing transcript information for the region
         */
        void addGeneTrack(const char* refName, unsigned int physicalStart, unsigned int physicalStop, std::vector<YTranscript*> *transcripts);
        
        void display(); //!< triggers rendering of the view
        void draw();    //!< draws the view
        
        //! finds the YPairView for a given referenceName and position
        /*! Currently regions are identified by their refName and position. Attempting to use this with multiple identical regions, but say different alignment files will result in incorrect behavior. 
        \param refName chromosome name
        \param pos start of the region
        \return a YPairView that contains the reference name and position
         */
        YPairView* findViewForLocation(const char* refName, unsigned int pos);
        
        //! draws read pairs within the appropriate YPairViews
        /*! The reads are drawn from YGenomeView to allow for easy drawing of translocations. Uses findViewForLocation to identify which YPairView the reads should be drawn in.
        \param mate the mate pair to draw
        \param leftView the YPairView containing the leftmost read
        \param rightView the YPairView containing the rightmost read
         */
        void drawReadPairInViews(YMatePair *mate, YPairView* leftView, YPairView* rightView);
        
        //! sets the color for a pair 
        void setColorForPair(YMatePair *pair);
        
        //! sets the color for the flag
        void setColorForFlag(unsigned int flag);
        
        //! sets whether to suppress the drawing of reads where both are mates are not mapped within the region 
        void setSuppressUnpairedReads(bool flag);
        
        //! returns whether the view is set to suppress the drawing of reads where both are mates are not mapped within the region 
        bool suppressingUnpairedReads();

        //! configures the colors as default for Illumina paired end libraries
        void setIlluminaColorScheme();
        
        //! configures the colors as default for SOLiD mate pair libraries
        void setSolidColorScheme();
};




#endif

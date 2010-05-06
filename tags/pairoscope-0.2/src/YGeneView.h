/*----------------------------------
  $Author$ 
  $Date$
  $Revision$
  $URL$
  -----------------------------------

  Enter Description Here

  ----------------------------------*/
#ifndef YGENEVIEW_H
#define YGENEVIEW_H

#include <YView.h>
#include <vector>
#include "YTranscript.h"

//! YGeneView displays gene exons across a region
/*! This class receives a reference to an array of transcripts across a region and then graphs them. */

class YGeneView : public YView
{
    std::vector<YTranscript*> *transcripts; //!< list of transcripts to display
    char *refName;                      //!< name of the chromosome/contig
    YRect plotArea;	                //!< The region where the depth will be plotted (basically what's left after drawing axes)
    double fontSize;                    //!< The font size to use in pixels
    unsigned int physicalStart;	        //!< where this graph begins on the reference
    unsigned int physicalStop;		//!< where this graph ends on the reference
    bool topTranscript;             //!< select the "best" transcript



    //! Returns a priority code for a genbank status 
    /*!
    \param status name of the status in question
    \return the priority of that status in selecting a transcript. Lower is better.
     */
    int transcriptStatusPriority(char *status);
    
public:
    //! constructs a new GeneView
    /*! 
    \param cr cairo context of the view
    \param initialFrame boundaries of the view within the parent
    \param refName reference name for axis label
    \param transcripts a reference to a YTranscript vector object
    \param fontSize the size of the font in screen coordinates
     */
    YGeneView(cairo_t *cr, YRect initialFrame, const char *refName, unsigned int physicalStart, unsigned int physicalStop, std::vector<YTranscript*> *transcriptVector, bool topTranscript = true, double fontSize = 14.0);

    ~YGeneView();
    
    //! draws a GeneView object
    void draw();
    
    //!calculates the size available for the plot and draws the axes
    void calculateAxes();	
    
    //! converts the boundaries of the graph, data region only, to the coordinates of the YView's parent
    /*!
    \return a YRect containing the bounds of the data region of the graph in parent coordinates
     */
    YRect plotAreaInParentCoordinates();
    
    //! sets the boundaries of the graph, data region only, to YRect passed with parent coordinates
    /*!
    \param newPlotArea YRect containing desired bounds of the graph data in this view's parent's coordinate system
    \return whether the setting was successful
     */
    bool setPlotAreaInParentCoordinates(YRect newPlotArea);
    

};

#endif

/*----------------------------------
  $Author$ 
  $Date$
  $Revision$
  $URL$
  ----------------------------------*/

#ifndef YPAIRVIEW_H
#define YPAIRVIEW_H

#include "YRect.h"
#include "YView.h"
#include <vector>

//! YPairView is a class for drawing read pairs in a "barcode" style
/*! It does not draw itself within the render tree, but provides methods to draw reads within its coordinate system on request.
*/
class YPairView : public YView {
    private:
    char *refName;                 //!< name of the chromosome/contig
    unsigned int physicalStart;	//!< where this graph begins on the reference
    unsigned int physicalStop;		//!< where this graph ends on the reference
    
    YRect plotArea;	//!< The region where the pairs will be plotted (basically what's left after drawing axes)
    cairo_text_extents_t start_text_bb;//!< variable to store the bounding boxes of the start axis label
    char *startLabelText; //!< string containing the starting coordinate text label
    cairo_text_extents_t stop_text_bb; //!< variable to store the bounding boxes of the stop axis label
    char *stopLabelText; //!< string containing the ending coordinate text label
    
    double readHeight; //!< height of reads in view's own coordinates
    double axisOffset; //!< distance in screen coordinates between the beginning of the plotArea and the axis labels
    double fontSize;   //!< size of the font in screen coordinates
    //! sets up the text label for the start position
    /*!
        \param label string to label the start position with
    */
    void setStartLabelText(char* label);
        
    //! sets up the text label for the stop position
    /*!
    \param label string to label the stop position with
     */
    void setStopLabelText(char* label);

public:
    
    //! constructs a new DepthView
    /*! 
        \param cr cairo context of the view
        \param initialFrame boundaries of the view within the parent
        \param refName reference name for axis label
        \param physicalStart the start position on the reference sequence (1-based) of the region
        \param physicalStop the end position on the reference sequence (1-based) of the region
        \param readHeight the height of the reads in the view's coordinate system
        \param fontSize the size of the font in screen coordinates
        \param axisOffset the distance between the labels and the axis in screen coordinates
    */
    YPairView(cairo_t *cr, YRect initialFrame, const char *refName, unsigned int physicalStart, unsigned int physicalStop, double readHeight = 0.25, double fontSize = 14.0, double axisOffset = 2.0);
    
    //! destroys YPairView
    ~YPairView();
    
    //! gives top point of read in own coordinates
    /*!
    \param coordinate the x-location within the view where the read is to be drawn.
    \return a YPoint object with the location of the point in the view's own coordinates
    */
    YPoint topPointOfRead(double coordinate);
    
    //! gives bottom point of read in own coordinates
    /*!
    \param coordinate the x-location within the view where the read is to be drawn.
    \return a YPoint object with the location of the point in the view's own coordinates
     */
    YPoint bottomPointOfRead(double coordinate);
    
    //! sets the start position of the graph on the reference sequence
    /*!
    \param start the new starting position of the graph.
     */
    void setPhysicalStart(unsigned int start);
    
    //! sets the stop position of the graph on the reference sequence
    /*!
    \param stop the new stop position of the graph.
     */
    void setPhysicalStop(unsigned int stop);
    
    
    void calculateAxes();	//!< calculates the size available for the plot and draws the axes
    void draw();                //!< draws the graph
    
    //! converts this point to the coordinates of the YView's parent
    /*!
    \param point the point to be converted in the YView's own coordinates
     */
    void pointInParentCoordinates(YPoint *point);
    
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
    
    //! returns the reference name for this view
    /*!
    \return a constant pointer to the refName
     */
    const char* getRefName();
    
    //! returns the start for this view
    unsigned int getPhysicalStart();
    
    //! returns the stop for this view
    unsigned int getPhysicalStop();
};

#endif

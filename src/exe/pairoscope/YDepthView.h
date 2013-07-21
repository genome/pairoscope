/*----------------------------------
  $Author$
  $Date$
  $Revision$
  $URL$
  -----------------------------------

  This is a view for displaying sequence depth over a chromosomal region

  ----------------------------------*/

#ifndef YDEPTHVIEW_H
#define YDEPTHVIEW_H

#include "YRect.h"
#include "YView.h"
#include <string>
#include <vector>

//! YDepthView graphs read depth across a region
/*! This class receives a reference to an array of depth values across a region and then graphs that region. */

class YDepthView : public YView {
    std::vector<int> *depth;        //!< array of a depth value for every bp in the region
    std::string refName;                 //!< name of the chromosome/contig

    unsigned int displayMaximumDepth;     //!< maximum value of y-axis

    YRect plotArea;	//!< The region where the depth will be plotted (basically what's left after drawing axes)

    bool autoScale;	//!< whether or not the view should set the Y-axis maximum based on the depth


    cairo_text_extents_t max_text_bb;   //!< the boundaries of the maximum y-value text label
    std::string maxTextLabel;                 //!< the maximum y-value text label
    cairo_text_extents_t min_text_bb;   //!< the boundaries of the minimum y-value text label (always 0)
    cairo_text_extents_t axis_text_bb;   //!< the boundaries of the minimum y-value text label (always 0)
    std::string axisTextLabel;                 //!< the minimum y-value text label (always 0)

    double fontSize;
    double axisOffset;

public:

    //! constructs a new PairView
    /*!
    \param cr cairo context of the view
    \param initialFrame boundaries of the view within the parent
    \param refName reference name for axis label
    \param depthVector a reference to an integer vector object
    \param fontSize the size of the font in screen coordinates
    \param axisOffset the distance between the y axis labels and the axis in screen coordinates
     */
    YDepthView(cairo_t *cr, YRect initialFrame, const char *refName, std::vector<int> *depthVector, double fontSize = 14.0, double axisOffset = 10.0);
    //! destroys a PairView object
    ~YDepthView();

    //! draws a PairView object
    void draw();

    //! sets the maximum depth
    /*!
    \param maxDepth the maximum depth to set the graph to
    */
    void setDisplayMaximumDepth(unsigned int maxDepth);

    //! sets whether the view sets the maximum depth value based on the calculated depth of the region
    /*!
    \param autoScale whether or not the view should automatically scale its y-axis
     */
    void setAutoScale(bool autoScale);

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

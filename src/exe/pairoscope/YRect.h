/*----------------------------------
  $Author$
  $Date$
  $Revision$
  $URL$
  -----------------------------------

  This is a simple rectangle struct

  ----------------------------------*/

#ifndef YRECT_H
#define YRECT_H

//! YRect is simple class to store a rectangle
/*! YRect provides storage for a rectangle. It is used as the basis for YView's frame and bounds. */

class YRect {
    public:
    double x; //!< x-coordinate of the origin
    double y; //!< y-coordinate of the origin
    double width; //!< width in pixels
    double height; //!< height in pixels

    //! Constructor for initialization of all parameters
    /*!
    \param x the x-coordinate of the YRect's origin
    \param y the y-coordinate of the YRect's origin
    \param width the width of the YRect
    \param height the height of the YRect
    \return a new YRect object
    */
    YRect(double x, double y, double width, double height) : x(x), y(y), width(width), height(height) { return; };
    //! Default constructor
    /*!
    \return a new YRect object
    */
    YRect() : x(0), y(0), width(0), height(0) {	return; };
};


#endif

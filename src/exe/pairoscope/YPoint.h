/*----------------------------------
  $Author$
  $Date$
  $Revision$
  $URL$
  -----------------------------------

  This is a simple point class

  ----------------------------------*/
#ifndef YPOINT_H
#define YPOINT_H
//! YPoint is a simple class to store points in 2-D space
/*! This is mainly for passing around points to various functions, but it may be incorporated into YRect at some point */
class YPoint {
    public:
        double x; //!< x-coordinate
        double y; //!< y-coordinate

        //! creates a new point with parameters
        /*!
        \param x the x-coordinate
        \param y the y-coordinate
        \return a new YPoint object
        */
        YPoint(double x, double y) : x(x), y(y) {return;};

        //!creates a point at the origin as the default
        /*!
        \return a new YPoint object
        */
        YPoint() : x(0.0), y(0.0) {return; };
};
#endif

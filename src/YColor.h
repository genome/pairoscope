/*----------------------------------
  $Author: ernfrid $ 
  $Date: 2010-05-06 13:41:51 -0500 (Thu, 06 May 2010) $
  $Revision: 1 $
  $URL: https://pairoscope.svn.sourceforge.net/svnroot/pairoscope/trunk/src/YRect.h $
  -----------------------------------

  This is a simple color class 

  ----------------------------------*/

#ifndef YCOLOR_H
#define YCOLOR_H

//! YColor is simple class to store an RGBA color
/*! YColor provides storage for a color. It is used as the basis for storing the arc colors. */

class YColor {
    public:
    double red; //!< red component of the color
    double green; //!< green component of the color
    double blue; //!< blue component of the color
    double alpha; //!< alpha component of the color

    //! Constructor for initialization of all parameters 
    /*! 
    \param r red component (between 0 and 1) 
    \param g green component (between 0 and 1)
    \param b blue component (between 0 and 1)
    \param a alpha component (between 0 and 1)
    \return a new YColor object
    */
    YColor(double r, double g, double b, double a) : red(r), green(g), blue(b), alpha(a) { return; };
    //! Default constructor
    /*!
    \return a new YColor object
    */
    YColor() : red(0), green(0), blue(0), alpha(1) {	return; };
};


#endif

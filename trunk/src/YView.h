/*----------------------------------
  $Author$ 
  $Date$
  $Revision$
  $URL$
  -----------------------------------

  This is a generic View graph to abstract some common graphical layout tasks

  ----------------------------------*/

#ifndef YVIEW_H
#define YVIEW_H

#include <cairo.h>
#include "YRect.h"
#include "YPoint.h"

//! YView is a simple view (or canvas) class
/*! YView is a simple class that aims to mimic Apple's NSView class in many respects. It operates on a cairo context and can draw itself on this context. YView is an "abstract base class" in that it will not draw anything on its own. The main function of the class is to automatically set up proper transforms for a rectagular coordinate system within the cairo context. YViews can also have child YViews that draw with respect to their parent YViews. Thus there is a rendering hierarchy where a parent is set up, it's children and their children are drawn and then the YView itself (a depth first traversal of the view tree */ 
class YView {
protected:    
    
    //Store cairo related data
    cairo_t *context; //!< cairo drawing context that the YView draws to
    cairo_matrix_t matrix;  //!< cairo matrix that defines the  transformations necessary to get to YView coordinates

    //Stealing this idea from Cocoa's NSView
    YRect frame; //!< location within superview. Screen coordinates otherwise.
    YRect bounds; //!< own coordinate system 

    //View Hierarchy related
    YView *parentView; //!< pointer to the parent of this View
    YView *siblingView; //!< pointer to the next sibling of this view
    YView *childView;   //!< pointer to the first child of this view
    
    //Setting as option to facilitate cross view drawing
    bool clipFrame; //!< whether or not the view clips attempted drawing outside its borders
    bool drawsSelf; //!< whether or not the view draws itself


    //! sets the transformation matrix for the view
    /*! Use this function to set up the view transformation manually
        \param newMatrix the new transformation matrix
    */
    void setMatrix(const cairo_matrix_t newMatrix);

    //! sets up this YView's coordinate system 
    void prepareForRender();
    
    //! initiates drawing code
    /*! This function will be moved to protected when the project progresses. It sets up the YView transformation matrix and then calls the YView's draw command. Basically, this serves to encapsulate transformation and context operations for the View. It is also where the drawing of childViews is initiated.
    \sa draw()
     */
    void render();
    
    //! restores to parent's coordinate system
    void endRender();
public:
    //Constuctors
    //! creates a new YView object
    /*! The cairo context is not owned by the object. If it is deleted or goes out of scope while the object exists then there will be a crash 
      
      \param cr the cairo context this YView draws upon.
      \param initialFrame the bounds of this View in its parent's coordinates
      \param willClipFrame whether or not the view will clip any drawing outside of its frame
      \param willDrawSelf whether or not the view will draw itself
      \return a new YView object
     */
    YView(cairo_t *cr, YRect initialFrame, bool willClipFrame = true, bool willDrawSelf = true);
    
    virtual ~YView();
    
    //methods
    //! adds a child YView to this view's render tree
    /*! The child view will be contained within the bounds of this view. This is a weak reference, so be careful the childView doesn't go out of scope or get deleted. Currently there is no function to remove a childView.
      
      \param newChildView the view to add
     */
    void addChildView(YView *newChildView);
    
    //! returns the YViews sibling YView
    /*! Useful for traversing the rendering hierarchy. If you know what you are doing.
        \return sibling YView
    */
    YView *getSiblingView(void);
    
    //! sets the frame
    /*! Dimensions of the YRect should be in the view parent's coordinates (or document coordinates if the view has no parents
    \param newFrame the dimensions of the new frame
    */
    void setFrame(YRect newFrame);
    
    //! returns the view's frame
    /*! 
    \return a YRect containing the dimensions of the view's frame
    */
    
    YRect getFrame(void);
    
    //! sets the bounds of the view.
    /*! The YView's bounds in its internal coordinate system. Normally this is the same as the YView's frame, but if set differently, it can be used to restrict only a sub-region of the YView's frame to draw in.
    \param newBounds the dimensions of the new bounds
    */
    void setBounds(YRect newBounds);
    
    //! returns the view's frame
    /*! 
    \return a YRect containing the dimensions of the view's bounds
     */
    YRect getBounds(void);
    
    //! converts this point to the coordinates of the YView's parent
    /*!
    \param point the point to be converted in the YView's own coordinates
    */
    virtual void pointInParentCoordinates(YPoint *point);
    
    //! converts this rect to the coordinates of the YView's parent
    /*!
    \param rect the point to be converted in the YView's own coordinates
     */
    virtual void rectInParentCoordinates(YRect *rect);
        
    //! drawing of the view occurs here
    /*! Any subclasses should override this method if they plan to do any drawing within the view. The YView will set up transformations so that the drawing coordinates are in the proper coordinate system.
    */
    
    virtual void draw();
    
    //! allows controlled access to childView hierarchy
    /*! Children are ordered by how they are added and deleted. This allows for access taking into account this fact and treating the children as a 0-indexed array. Out of bounds requests will simply return the null pointer. This is pretty inefficient. You may want to use getSiblingView() and do this yourself if you are concerned about efficiency.
    
    \param index the index of the child to retrieve
    \return a YView pointer to the requested child or null if none exists
    \sa getSiblingView()
    */
    YView* childAtIndex(unsigned int index);
    
    //!inserts a new child view after the passed index
    /*! Inserts a view after the proposed index. If the index does not exist then no operation will occur and the function will return false.
    \param index the index of the child to insert after
    \param child the view to insert as a child
    \return a boolean indicating whether the insertion was successful
    */
    bool insertChildAfterIndex(unsigned int index, YView* newChild);
};


#endif

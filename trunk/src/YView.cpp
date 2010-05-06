/*----------------------------------
  $Author$ 
  $Date$
  $Revision$
  $URL$
  -----------------------------------

  This is a generic View graph to abstract some common graphical layout tasks

  ----------------------------------*/

#include "YView.h"
#include <cstdlib> //for NULL definition

//Constructor, may want to consider simplifying out some of the types
YView::YView(cairo_t *cr, YRect frame, bool willClipFrame, bool willDrawSelf) 
    :   context(cr), 
        frame(frame),
        parentView(NULL),
        siblingView(NULL),
        childView(NULL),
        clipFrame(willClipFrame),
        drawsSelf(willDrawSelf)
    
{
    cairo_matrix_init_identity(&matrix);
    setBounds(YRect(0,0,frame.width, frame.height));    //by default set the Bounds to the Frame
    return;
}

YView::~YView() {
    if(childView) {
        //iterate over siblings of child
        YView *lastView = childView;
        YView *viewToDelete = NULL;
        while(lastView->siblingView) {
            viewToDelete = lastView;
            lastView = lastView->siblingView;
            delete viewToDelete;
        }
        delete lastView;
    }
}

void YView::setMatrix(const cairo_matrix_t newMatrix) {
    matrix = newMatrix;
    setFrame(frame);
} 

void YView::addChildView(YView *newChildView) {
    //find the next available sibling
    if(childView) {
        //iterate over siblings of child
        YView *lastView = childView;
        while(lastView->siblingView) {
            lastView = lastView->siblingView;
        }
        lastView->siblingView = newChildView;
    }
    else {
        childView = newChildView;
    }
}

YView* YView::getSiblingView(void) {
    return this->siblingView;    
}

void YView::setFrame(YRect newFrame) {
    //may affect clipping if we set a clipping rect to make the view respect it's boundaries
    frame = newFrame;
    setBounds(bounds); //reset scaling etc, sets up matrix with new frame

    //clipping should be done within the draw routine
}

YRect YView::getFrame() {
    return frame;
}

void YView::setBounds(YRect newBounds) {
    bounds = newBounds;
    cairo_matrix_init_translate(&matrix, frame.x, frame.y);
    
    //translate to new origin
    cairo_matrix_translate(&matrix, bounds.x, bounds.y);
    //rescale the matrix.
    cairo_matrix_scale(&matrix,frame.width/bounds.width, frame.height/bounds.height);
}

YRect YView::getBounds() {
    return bounds;
}

void YView::pointInParentCoordinates(YPoint *point) {
    //This is counterintuitive, but you need to re-transform the point by the View's transformation matrix
    cairo_matrix_t undo_matrix;
    cairo_matrix_init_translate(&undo_matrix, frame.x, frame.y);
    cairo_matrix_translate(&undo_matrix, bounds.x, bounds.y);
    cairo_matrix_scale(&undo_matrix, frame.width/bounds.width, frame.height/bounds.height);


    cairo_matrix_transform_point(&undo_matrix, &(point->x), &(point->y));
}

void YView::rectInParentCoordinates(YRect *rect) {
    //silly to replicate this code but why not!
    cairo_matrix_t undo_matrix;
    cairo_matrix_init_translate(&undo_matrix, frame.x, frame.y);
    cairo_matrix_translate(&undo_matrix, bounds.x, bounds.y);
    cairo_matrix_scale(&undo_matrix, frame.width/bounds.width, frame.height/bounds.height);
    
   
    cairo_matrix_transform_point(&undo_matrix, &(rect->x), &(rect->y));
    cairo_matrix_transform_distance(&undo_matrix, &(rect->width), &(rect->height));
}

YView* YView::childAtIndex(unsigned int index) {
    YView* currentView = this->childView;
    unsigned int i = 0; //counter
    while(i < index && currentView) {
        currentView = currentView->siblingView;
        ++i;
    }
    return currentView;
}

bool YView::insertChildAfterIndex(unsigned int index, YView* newChild) {
    YView* currentView = this->childAtIndex(index);
    if(currentView) {
        YView *tempView = currentView->siblingView;
        currentView->siblingView = newChild;
        newChild->siblingView = tempView;
        return true;
    }
    else {
        return false;
    }
}

void YView::draw() {
    //this should be overridden by child classes to do drawing
}

void YView::render() {
    prepareForRender();
    
    if(drawsSelf) {
        draw(); //render self
    }
    //render children
    YView *currentView = childView;
    while(currentView) {
        currentView->render();
        currentView = currentView->siblingView;
    }
    
    endRender();
}

void YView::prepareForRender() {
    //assume that our parent has set up the context for us
    cairo_save(context);
    
    if(clipFrame) {
	//set up clipping within parentView
        cairo_rectangle(context,frame.x, frame.y, frame.width, frame.height);
        cairo_clip(context); //set up clipping so we respect our elders
    }

    cairo_transform(context, (const cairo_matrix_t*) &matrix); //set up this view's matrix
}

void YView::endRender() {
    //restore parent's matrix
    cairo_restore(context);
}



    

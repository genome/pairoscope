/*----------------------------------
$Author$
$Date$
$Revision$
$URL$
----------------------------------*/

#include <string>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <cstdio>
#include "YPairView.h"

using std::stringstream;

YPairView::YPairView(cairo_t *cr, YRect initialFrame, const char *refName, unsigned int physicalStart, unsigned int physicalStop, double readHeight, double fontSize, double axisOffset)
    : YView(cr, initialFrame, false, true)
    , refName(0)
    , physicalStart(physicalStart)
    // make sure physicalStop is initialized before calling setPhysicalStart
    // since it depends on this value
    , physicalStop(physicalStop)
    , startLabelText("")
    , stopLabelText("")
{
    int length = strlen(refName);
    this->refName = new char[length + 1]; //create space for a copy of the refName
    strcpy(this->refName, refName);
    setPhysicalStart(physicalStart);
    setPhysicalStop(physicalStop);
    this->readHeight = readHeight;
    this->fontSize = fontSize;
    this->axisOffset = axisOffset;

    setBounds(YRect(0,0,physicalStop-physicalStart,1));
    return;
}

YPairView::~ YPairView() {
    if(refName) {
        delete[] refName;
    }
}

YPoint YPairView::topPointOfRead(double coordinate) {
    return YPoint(coordinate - physicalStart,bounds.height - readHeight);
}

YPoint YPairView::bottomPointOfRead(double coordinate) {
    return YPoint(coordinate - physicalStart, bounds.height);
}

void YPairView::setPhysicalStart(unsigned int start) {
    physicalStart = start;
    setBounds(YRect(0,0,physicalStop-physicalStart,1));
    stringstream label;
    label << this->refName << ":" << start;
    startLabelText = label.str();
}

void YPairView::setPhysicalStop(unsigned int stop) {
    physicalStop = stop;
    setBounds(YRect(0,0,physicalStop-physicalStart,1));
    stringstream label;
    label << this->refName << ":" << stop;
    stopLabelText = label.str();
}

void YPairView::pointInParentCoordinates(YPoint *point) {
    cairo_matrix_t undo_matrix;
    cairo_matrix_init_translate(&undo_matrix, plotArea.x, plotArea.y);
    cairo_matrix_scale(&undo_matrix, plotArea.width/bounds.width, plotArea.height/bounds.height);
    cairo_matrix_transform_point(&undo_matrix, &(point->x), &(point->y));
    YView::pointInParentCoordinates(point);
}

void YPairView::calculateAxes() {
    //first generate text and sizes
    //add in the offset
    //store the available space for the actual graph somewhere
    //drawing code should be extrapolated out of here, in order to allow for us to set the plot area outside of the object ie for aligning margins

    //For this view we only print the x axis for now
    //ADDED Add in some sort of offset between the labels and the axis (ie pad the plot area). Offset should be in the same coord system as font
    //TODO Add in tick marks, same rules apply as above
    //TODO Add in axis label
    //FIXME seems to be using plotArea incorrectly

    setBounds(YRect(0,0,physicalStop-physicalStart,1));

    cairo_save(context);
    cairo_identity_matrix(context);
        //Here is some font related crap
    cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD); //hardcoding for now
    cairo_set_font_size(context, fontSize);	//hardcoding for now


    cairo_text_extents(context, startLabelText.c_str(), &start_text_bb);	//this grabs the bounds of the start string
    cairo_text_extents(context, stopLabelText.c_str(), &stop_text_bb);	//this grabs the bounds of the stop string


    //this is sort of underhanded, but set up the transformation here
    prepareForRender();

    //move bounding width into local coordinates, this should be re-factored into a function
    //this seems dumb
    double dummy = 0;
    double user_offset = axisOffset;
    cairo_device_to_user_distance(context, &start_text_bb.width, &dummy);
    dummy = 0;
    cairo_device_to_user_distance(context, &dummy, &user_offset);
    dummy = 0;
    cairo_device_to_user_distance(context, &stop_text_bb.width, &dummy);
    dummy = 0;
    cairo_device_to_user_distance(context, &stop_text_bb.x_advance, &dummy);
    dummy = 0;
    cairo_device_to_user_distance(context, &dummy, &start_text_bb.height);
    dummy = 0;
    cairo_device_to_user_distance(context, &dummy, &stop_text_bb.height);


    //adjust plotArea
    plotArea.x = bounds.x + start_text_bb.width/2; //centering
    plotArea.width = bounds.width - plotArea.x - stop_text_bb.width/2;
    plotArea.y = bounds.y - user_offset;
    plotArea.height = bounds.height - start_text_bb.height - user_offset;

    endRender();
    cairo_restore(context);

}

void YPairView::draw() {

    //Draw the text labels
    //align to bottom left

    cairo_move_to(context, bounds.x, bounds.y + bounds.height);

    //preserve original scaling etc and draw the max label
    cairo_save(context);
    cairo_identity_matrix(context);	//scale to square pixels
    cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(context, fontSize);
    cairo_set_source_rgb(context, 0, 0, 0);  //draw in black
    cairo_show_text(context, startLabelText.c_str());

    cairo_restore(context);

    //draw at bottom right
    cairo_move_to(context, bounds.x + bounds.width - stop_text_bb.x_advance, bounds.y + bounds.height);

    //preserve original scaling etc and draw the max label
    cairo_save(context);
    //Here is some font related crap
    cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD); //hardcoding for now
    cairo_set_font_size(context, fontSize);
    cairo_identity_matrix(context);	//scale to actual size of document
    cairo_set_source_rgb(context, 0, 0, 0);  //draw in black
    cairo_show_text(context, stopLabelText.c_str());

    cairo_restore(context);


    //Now draw the axes
    cairo_save(context);
    cairo_translate(context, plotArea.x, plotArea.y);   //FIXME This is treating plotArea as an offset from y-transformed bounds WTF
    cairo_scale(context, plotArea.width/bounds.width, plotArea.height/bounds.height);
    cairo_set_source_rgb(context, 0, 0, 0);  //draw black line

    cairo_move_to(context,0,bounds.height); //move to the origin
    cairo_line_to(context,bounds.width,bounds.height);	//plot horizontal line
        //cairo_move_to(context,0,plotArea.y + plotArea.height); //move to the origin
        //cairo_rel_line_to(context,0, 5);	//hardcoded tick mark
        //cairo_rel_move_to(context, plotArea.width,0);
        //cairo_rel_line_to(context,0, -10);	//hardcoded
    cairo_restore(context);

    cairo_save(context);

    cairo_identity_matrix(context);	//scale to actual size of document
    cairo_set_source_rgb(context, 0, 0, 0);  //draw in black
    cairo_stroke(context);

    cairo_restore(context);
}


YRect YPairView::plotAreaInParentCoordinates() {
    YRect returnPlotArea = this->plotArea;
    YView::rectInParentCoordinates(&returnPlotArea);
    return returnPlotArea;
}

bool YPairView::setPlotAreaInParentCoordinates(YRect newPlotArea) {
    cairo_matrix_t conversion_matrix = matrix;
    if(cairo_matrix_invert(&conversion_matrix) == CAIRO_STATUS_SUCCESS) {
        cairo_matrix_transform_point(&conversion_matrix, &newPlotArea.x, &newPlotArea.y);
        cairo_matrix_transform_distance(&conversion_matrix, &newPlotArea.width, &newPlotArea.height);
        this->plotArea = newPlotArea;
        return true;
    }
    else {
        return false;
    }
}

const char* YPairView::getRefName() {
    return (const char*) this->refName;
}

//! returns the start for this view
unsigned int YPairView::getPhysicalStart() {
    return physicalStart;
}

//! returns the stop for this view
unsigned int YPairView::getPhysicalStop() {
    return physicalStop;
}

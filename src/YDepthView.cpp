/*----------------------------------
  $Author$ 
  $Date$
  $Revision$
  $URL$
  -----------------------------------

  YDepthView graphs the read depth across a region

  ----------------------------------*/

#include "YDepthView.h"
#include <algorithm>
#include <math.h>
#include <iostream>
#include <string.h>
#include <stdio.h>

YDepthView::YDepthView(cairo_t *cr, YRect initialFrame, const char *refName, std::vector<int> *depthVector, double fontSize, double axisOffset) : maxTextLabel(NULL), axisTextLabel(NULL), YView(cr,initialFrame), depth(depthVector), refName(NULL) {
    int length = strlen(refName);
    this->refName = new char[length + 1]; //create space for a copy of the refName
    strcpy(this->refName, refName);
    asprintf(&this->axisTextLabel,"Depth for %s", this->refName);
    this->fontSize = fontSize;
    this->axisOffset = axisOffset;
    
}

YDepthView::~YDepthView() {
    if(refName) {
        delete[] refName;
    }
    if(maxTextLabel) {
        free(maxTextLabel);
    }
    if(axisTextLabel) {
        free(axisTextLabel);
    }
}

void YDepthView::draw() {
    //first calculate the spacing again in case the plotArea has changed
    double dummy = 0;
    double offset = axisOffset;
    cairo_device_to_user_distance(context, &offset, &dummy);
    //then we will draw them 
    cairo_move_to(context, plotArea.x-max_text_bb.width - offset, bounds.y + max_text_bb.height);	
    //align with top left part of DepthView bounds
    
    //preserve original scaling etc and draw the max label
    cairo_save(context);
    
        cairo_identity_matrix(context);	//scale to actual size of document
        cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD); //hardcoding for now
        cairo_set_font_size(context, fontSize);	//hardcoding for now
        cairo_set_source_rgb(context, 0, 0, 0);  //draw in black
        cairo_show_text(context, maxTextLabel);
   
    cairo_restore(context);
    
    //now do minimum number (for now always zero)
    
    //right align to the max label
    cairo_move_to(context, plotArea.x - min_text_bb.width - offset, bounds.y + bounds.height);
    
    //preserve original scaling etc and draw the max label
    cairo_save(context);
    
        cairo_identity_matrix(context);	//scale to actual size of document
        cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD); //hardcoding for now
        cairo_set_font_size(context, fontSize);	//hardcoding for now
        cairo_set_source_rgb(context, 0, 0, 0);  //draw in black
        cairo_show_text(context, "0");
   
    cairo_restore(context);
    
    cairo_move_to(context, (bounds.x + axis_text_bb.height + offset), bounds.y + bounds.height/2);
    cairo_save(context);
        cairo_rotate(context,-1.57079633);
        cairo_rel_move_to(context,-(axis_text_bb.width)/2.0,0);
        cairo_identity_matrix(context);	//scale to actual size of document
        cairo_rotate(context,-1.57079633);
        cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD); //hardcoding for now
        cairo_set_font_size(context, fontSize);	//hardcoding for now
        cairo_set_source_rgb(context, 0, 0, 0);  //draw in black
        cairo_show_text(context, axisTextLabel);
    cairo_restore(context);    
    
    //Now draw the axes 
    cairo_save(context);
        cairo_translate(context, plotArea.x, plotArea.y);
        cairo_scale(context, plotArea.width/bounds.width, plotArea.height/bounds.height);
        cairo_set_source_rgb(context, 0, 0, 0);  //draw black line
        cairo_move_to(context,bounds.x,bounds.y + bounds.height); //move to the origin
        cairo_line_to(context,bounds.x + bounds.width,bounds.y + bounds.height);	//plot horizontal line   
        cairo_move_to(context,bounds.x,bounds.y + bounds.height); //move to the origin
        cairo_line_to(context,bounds.x,bounds.y);
        //cairo_rel_move_to(context,-5,0);	//hardcoded tick mark
        //cairo_rel_line_to(context,10,0);	//hardcoded
        //cairo_move_to(context,plotArea.x,plotArea.y+plotArea.height);
        //cairo_rel_line_to(context,-5,0);
        
        //preserve original scaling etc and draw the max label
        cairo_save(context);
            
            cairo_identity_matrix(context);	//scale to actual size of document

            cairo_set_source_rgb(context, 0, 0, 0);  //draw in black
            cairo_stroke(context);
        
        cairo_restore(context);
        
        //then we will draw the read depth
        
        cairo_translate(context, 0, bounds.height);//translate so that we are drawing up the way humans look at a graph, and from the origin
        //Now graph the depth
        //first find the dynamic range of the array
        
    
        //scale to dynamic range (always assuming min is 0)
        //cairo_scale( context, bounds.width/(depth->size()-1), bounds.height/displayMaximumDepth);
        
        //iterate over the depth and draw that bad boy
        unsigned int i;
        for(i = 0; i != depth->size(); i++) {
            cairo_line_to(context, i, -(*depth)[i]);
        }
        
        //preserve original scaling etc and draw the depth graph
        cairo_save(context);
            
            cairo_identity_matrix(context);	//scale to actual size of document
            cairo_set_source_rgb(context, 0, 0, 0);  //draw in black
            cairo_set_line_width(context, 1.0);
            cairo_stroke(context);
    
        cairo_restore(context);
    cairo_restore(context);
    
    

}

void YDepthView::setDisplayMaximumDepth(unsigned int maxDepth) {
    this->displayMaximumDepth = maxDepth;
    if(maxTextLabel) {
        free(maxTextLabel);
    }
    asprintf(&maxTextLabel,"%d",this->displayMaximumDepth);	//This converts the integer number of the maximum to a string
}

void YDepthView::setAutoScale(bool autoScale) {
    this->autoScale = autoScale;
}

void YDepthView::calculateAxes() {
    //first generate text and sizes
    //add in the offset
    //store the available space for the actual graph somewhere 
    if(autoScale) {
        int max = 0;
        if(depth->empty()) {
            max = 1;
        }
        else {
            if(depth->size() == 1) {
                max = depth->at(0); //for some crazy reason I don't understand. Max element is crashing when there is only a single element
            }
            else {
                max = *max_element(depth->begin(),depth->end());
            }
        }
        setDisplayMaximumDepth(max);
    }
    
    //For the depth view we won't print X labels, so just calculate the space needed for the Y axis
    setBounds(YRect(0,0,depth->size()-1,displayMaximumDepth));
    cairo_save(context);
    cairo_identity_matrix(context);
    //Here is some font related crap
    cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD); //hardcoding for now
    cairo_set_font_size(context, fontSize);	//hardcoding for now

    cairo_text_extents(context, maxTextLabel, &max_text_bb);	//this grabs the bounds of the number string
    cairo_text_extents(context, "0", &min_text_bb);
    //cairo_rotate(context,-1.57079633);
    cairo_text_extents(context, axisTextLabel, &axis_text_bb);
    cairo_restore(context);
    
    prepareForRender();
    double dummy = 0;
    double offset = axisOffset;
    cairo_device_to_user_distance(context, &max_text_bb.width,&dummy);
    dummy = 0;
    cairo_device_to_user_distance(context, &dummy,&max_text_bb.height);  //change into user coordinates
    dummy = 0;
    cairo_device_to_user_distance(context, &min_text_bb.width,&dummy);
    dummy = 0;
    cairo_device_to_user_distance(context, &dummy,&min_text_bb.height);
    dummy = 0;
    cairo_device_to_user_distance(context, &offset, &dummy);
    dummy = 0;
    cairo_save(context);
        
        dummy = 0;
        cairo_device_to_user_distance(context, &axis_text_bb.height, &dummy);
        dummy = 0;
        cairo_device_to_user_distance(context, &axis_text_bb.y_advance, &dummy);
        dummy = 0;
        cairo_device_to_user_distance(context, &dummy, &axis_text_bb.width);
        cairo_device_to_user_distance(context, &dummy, &axis_text_bb.x_advance);
        
    cairo_restore(context);    
    
    endRender();
    
    
    
    //adjust plotArea
    plotArea.x = bounds.x + (max_text_bb.width + max_text_bb.x_advance) + offset;// + max_text_bb.height;
    plotArea.width = bounds.width - plotArea.x - offset;
    plotArea.y = bounds.y + max_text_bb.height/2;
    plotArea.height = bounds.height - plotArea.y - min_text_bb.height/2;

}

YRect YDepthView::plotAreaInParentCoordinates() {
    cairo_matrix_t undo_matrix;
    YRect returnPlotArea = this->plotArea;
    YView::rectInParentCoordinates(&returnPlotArea);
    return returnPlotArea;
}

bool YDepthView::setPlotAreaInParentCoordinates(YRect newPlotArea) {
    cairo_matrix_t conversion_matrix = matrix;
    if(cairo_matrix_invert(&conversion_matrix) == CAIRO_STATUS_SUCCESS) {
        cairo_matrix_transform_point(&conversion_matrix, &newPlotArea.x, &newPlotArea.y);
        cairo_matrix_transform_distance(&conversion_matrix, &newPlotArea.width, &newPlotArea.height);
        this->plotArea = newPlotArea;
    }
    else {
        return false;
    }
}

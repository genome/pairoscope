/*----------------------------------
  $Author$ 
  $Date$
  $Revision$
  $URL$
  ----------------------------------*/

#include "YGenomeView.h"
#include <cstddef>
#include <iostream>
#include <cstring>
#include <cstdio>

void YGenomeView::addRegion(const char* refName, unsigned int physicalStart, unsigned int physicalStop, std::vector<int> *depth) {
    //each region will have two tracks
    YDepthView *newDepthView = new YDepthView(context, YRect(), refName, depth);
    newDepthView->setAutoScale(true);
    YPairView *newPairView = new YPairView(context, YRect(), refName, physicalStart, physicalStop);
    int numberNonGeneTracks = numberTracks - geneTracks;
    if(childView) {
        this->insertChildAfterIndex(numberNonGeneTracks/2 - 1, newDepthView); 
    }
    else {
        addChildView(newDepthView);
    }
    //add final pairView
    this->insertChildAfterIndex(numberNonGeneTracks, newPairView);
    
    numberTracks += 2;
    layoutTracks();
    
}


void YGenomeView::addGeneTrack(const char* refName, unsigned int physicalStart, unsigned int physicalStop, std::vector<YTranscript*> *transcripts) {
    YGeneView *geneView = new YGeneView(context, YRect(), refName, physicalStart, physicalStop, transcripts, false);
    addChildView(geneView);
    geneTracks++;
    numberTracks++;
    layoutTracks();    
}

void YGenomeView::layoutTracks() {
    //divide up the tracks
    double spacing = 10.0;
    double trackHeight = (frame.height - (spacing*(numberTracks)))/numberTracks;
    
    //variable to store where axes should be drawn
    //note that I am only considering the case where you have a single column
    double best_xstart = bounds.x, best_width = bounds.width;
    
    int numberNonGeneTracks = numberTracks - geneTracks;

            
    //resize each childView 
    YView *currentView = childView;
    for(int i = 0; i < numberNonGeneTracks/2; ++i) {
        currentView->setFrame(YRect(frame.x, frame.y + trackHeight*i + spacing*i,frame.width,trackHeight));
        currentView->setBounds(YRect(frame.x, frame.y + trackHeight*i + spacing*i,frame.width,trackHeight));
        ((YDepthView*) currentView)->calculateAxes();
        YRect tempArea = ((YDepthView*) currentView)->plotAreaInParentCoordinates();
        //only aligning x coordinates
        if(best_xstart < tempArea.x) {
            best_xstart = tempArea.x;
        }
        if(best_width > tempArea.width) {
            best_width = tempArea.width;
        }
        
        currentView = currentView->getSiblingView();
    }
    for(int i = numberNonGeneTracks/2; i < numberNonGeneTracks; ++i) {
        currentView->setFrame(YRect(frame.x, frame.y + trackHeight*i+spacing*i,frame.width,trackHeight));
        currentView->setBounds(YRect(frame.x, frame.y + trackHeight*i+spacing*i,frame.width,trackHeight));
        ((YPairView*) currentView)->calculateAxes();
        YRect tempArea = ((YPairView*) currentView)->plotAreaInParentCoordinates();
        if(best_xstart < tempArea.x) {
            best_xstart = tempArea.x;
        }
        if(best_width > tempArea.width) {
            best_width = tempArea.width;
        }
        currentView = currentView->getSiblingView();
    }
    for(size_t i = numberNonGeneTracks; i < numberTracks; ++i) {
        currentView->setFrame(YRect(frame.x, frame.y + trackHeight*i+spacing*i,frame.width,trackHeight));
        currentView->setBounds(YRect(frame.x, frame.y + trackHeight*i+spacing*i,frame.width,trackHeight));
        ((YGeneView*) currentView)->calculateAxes();
        YRect tempArea = ((YGeneView*) currentView)->plotAreaInParentCoordinates();
        if(best_xstart < tempArea.x) {
            best_xstart = tempArea.x;
        }
        if(best_width > tempArea.width) {
            best_width = tempArea.width;
        }
        ((YGeneView*) currentView)->calculateAxes(); //should not be null   
        currentView = currentView->getSiblingView();
    }
    
    //NOw that we've gotten the initial layout, alter so that the axes line up for each view regardless of label size
    currentView = childView;
    for(int i = 0; i < numberNonGeneTracks/2; ++i) {
        
        YRect tempArea = ((YDepthView*) currentView)->plotAreaInParentCoordinates();
        tempArea.x = best_xstart;
        tempArea.width = best_width;
        ((YDepthView*) currentView)->setPlotAreaInParentCoordinates(tempArea);
        currentView = currentView->getSiblingView();
    }
    for(int i = numberNonGeneTracks/2; i < numberNonGeneTracks; ++i) {
        
        YRect tempArea = ((YPairView*) currentView)->plotAreaInParentCoordinates();
        tempArea.x=best_xstart;
        tempArea.width = best_width;
        ((YPairView*) currentView)->setPlotAreaInParentCoordinates(tempArea);
        currentView = currentView->getSiblingView();
    }
    for(size_t i = numberNonGeneTracks; i < numberTracks; ++i) {
        YRect tempArea = ((YGeneView*) currentView)->plotAreaInParentCoordinates();
        tempArea.x=best_xstart;
        tempArea.width = best_width;
        ((YGeneView*) currentView)->setPlotAreaInParentCoordinates(tempArea);
        currentView = currentView->getSiblingView();
    }
    
}

void YGenomeView::display() {
    this->render(); //start render tree

}

void YGenomeView::draw() {
    //Iterate over mates vector
    //Foreach, find the view for the left and right coords
    //Get coordinates
    //Draw
    std::vector<YMatePair*>::iterator i;
    for(i = mates->begin(); i != mates->end(); ++i) {
        //FIXME this should always be true, but testing for now
        if((*i)->leftRefName) {
            YPairView *leftPairView = findViewForLocation((*i)->leftRefName,(*i)->leftReadPosition);
            YPairView *rightPairView = NULL;
            if((*i)->rightRefName) {
                rightPairView = findViewForLocation((*i)->rightRefName, (*i)->rightReadPosition);
                if(strcmp((*i)->rightRefName, (*i)->leftRefName) != 0 && rightPairView) {
                    if(rightPairView) {
                        fprintf(stderr,"%s\t%d\t%s\t%d\n",(*i)->leftRefName,(*i)->leftReadPosition, (*i)->rightRefName, (*i)->rightReadPosition);
                    }
                }
            }
            if(leftPairView && (!suppressUnpaired || rightPairView)) {
            //draw the read 
                drawReadPairInViews(*i,leftPairView, rightPairView);
            }
        }
    }
}

YPairView* YGenomeView::findViewForLocation(const char* refName, unsigned int pos) {
    //FIXME this doesn't allow same regions with different alignment files
    int numberNonGeneTracks = numberTracks - geneTracks;    //exclude gene views
    
    YView* currentView = childView;
    //Skip DepthViews
    for(int i = 0; i < numberNonGeneTracks/2; ++i) {
        currentView = currentView->getSiblingView();
    }
    
    YPairView* currentPairView = (YPairView*) currentView;
    for(int i = numberNonGeneTracks/2; i < numberNonGeneTracks; ++i) {
        
        if(strcmp(currentPairView->getRefName(),refName) == 0 && pos >= currentPairView->getPhysicalStart() && pos <= currentPairView->getPhysicalStop()) {
            return currentPairView;
        }
        currentPairView = (YPairView*) currentPairView->getSiblingView();
    }
    return NULL;
    
}

void YGenomeView::drawReadPairInViews(YMatePair *mate, YPairView* leftView, YPairView* rightView) {
    cairo_save(context);
    if(leftView) {
        YPoint ltop = leftView->topPointOfRead(mate->leftReadPosition);
        leftView->pointInParentCoordinates(&ltop);
        YPoint lbottom = leftView->bottomPointOfRead(mate->leftReadPosition);
        leftView->pointInParentCoordinates(&lbottom);
        setColorForPair(mate);
        if(rightView) {
            YPoint rtop = rightView->topPointOfRead(mate->rightReadPosition);
            rightView->pointInParentCoordinates(&rtop);
            YPoint rbottom = rightView->bottomPointOfRead(mate->rightReadPosition);
            rightView->pointInParentCoordinates(&rbottom);
            
            if(rtop.y > ltop.y) {
                cairo_move_to(context, rbottom.x, rbottom.y - 1.0);
                cairo_line_to(context, rtop.x, rtop.y);
                cairo_curve_to(context, rtop.x,rightView->getFrame().y, ltop.x, leftView->getFrame().y, ltop.x, ltop.y );
                cairo_line_to(context,lbottom.x, lbottom.y - 1.0);
            }
            else {
                cairo_move_to(context, lbottom.x, lbottom.y - 1.0);
                cairo_line_to(context, ltop.x, ltop.y);
                cairo_curve_to(context, ltop.x,leftView->getFrame().y, rtop.x, rightView->getFrame().y, rtop.x, rtop.y );
                cairo_line_to(context,rbottom.x, rbottom.y - 1.0);
            }
        }
        else {
            cairo_move_to(context, lbottom.x, lbottom.y);
            cairo_line_to(context, ltop.x, ltop.y);
        }
        cairo_save(context);
        cairo_identity_matrix(context);
        cairo_set_line_width(context,1.0);
        cairo_stroke(context);
        cairo_restore(context);
    }
    cairo_restore(context);
}

void YGenomeView::setColorForPair(YMatePair* pair) {
    std::map<YMatePair::orientation_flag,YColor>::iterator pos;
    pos = colorMap.find(pair->orientation);
    if(pos != colorMap.end()) {
        YColor color = pos->second;
        cairo_set_source_rgba(context,color.red,color.green,color.blue,color.alpha); 
    }
    else {
        //for all other reads graph as darker gray
        cairo_set_source_rgba (context,0.8 , 0.8, .8,.8);
    }        
}

void YGenomeView::setColorForFlag(unsigned int flag) {
    if(flag == 18) {
        //Normal mapping
        //Draw as light gray boxes
        cairo_set_source_rgba (context,.9 , .9, .9,.8);
    }
    else if(flag == 192 || flag == 64) {
        //one was unplaced
        //Draw as a yellow box
        cairo_set_source_rgba (context,.8 , .8, 0,.8);
    }
    else if(flag == 1) {
        //mapped forward forward
        cairo_set_source_rgba (context,.8 , 0, 0,.8);
    }
    else if(flag == 4) {
        //Mapped RF
        cairo_set_source_rgba (context,0 , .8, 0,.8);
        
    }
    else if(flag == 8) {
        //Mapped RR
        cairo_set_source_rgba (context,0 , 0, .8,.8);
        
    }
    else if(flag == 130) {
        //one mapped by Smith-Waterman
        //Draw as black box
        cairo_set_source_rgba (context,0 , 0, 0,.8);
    }
    else if(flag == 32) {
        //different chromosomes
        cairo_set_source_rgba (context,0 , .8, .8,.8);

    }
    else if(flag == 2) {
        //mapped correctly
        cairo_set_source_rgba (context,.8 , 0, .8,.8);
    }
    else if(flag == 0) {
        //should be a fragment read
        cairo_set_source_rgba(context, .8, .8, .8, .8); //dark gray
    }
    else if(flag == 20) {
        //normally mapped if long insert library
        cairo_set_source_rgba(context, .93, .84, .72, .8); //light grey-orange
    }
    else {
        std::cerr << "Unhandled flag" << flag << "\n";
    }        
    
}
void YGenomeView::setSuppressUnpairedReads(bool flag) {
    this->suppressUnpaired = flag;
}

bool YGenomeView::suppressingUnpairedReads() {
    return this->suppressUnpaired;
}

void YGenomeView::setIlluminaColorScheme() {
    //populate the scheme
    colorMap.insert(std::make_pair(YMatePair::CT,YColor(0.0, 0.8, 0.8, 0.8))); //ctx
    colorMap.insert(std::make_pair(YMatePair::FR,YColor(0.9, 0.9, 0.9, 0.8))); //normal
    colorMap.insert(std::make_pair(YMatePair::PM,YColor(0.8, 0.8, 0.0, 0.8))); //partial match
    colorMap.insert(std::make_pair(YMatePair::FF,YColor(0.8, 0.0, 0.0, 0.8))); //inversion
    colorMap.insert(std::make_pair(YMatePair::RF,YColor(0.0, 0.8, 0.0, 0.8))); //inversion
    colorMap.insert(std::make_pair(YMatePair::RR,YColor(0.0, 0.0, 0.8, 0.8))); //inversion
    colorMap.insert(std::make_pair(YMatePair::IN,YColor(0.8, 0.25, 0.0, 0.8)));//insertion
    colorMap.insert(std::make_pair(YMatePair::DL,YColor(0.5, 0.0, 0.5, 0.8)));//deletion
    //colorMap.insert(std::make_pair(YMatePair::NF,YColor(0.5, 0.0, 0.5, 0.8)));
}

void YGenomeView::setSolidColorScheme() {
    //populate the scheme
    colorMap.insert(std::make_pair(YMatePair::CT,YColor(0.0, 0.8, 0.8, 0.8))); //ctx
    colorMap.insert(std::make_pair(YMatePair::FF,YColor(0.9, 0.9, 0.9, 0.8))); //normal
    colorMap.insert(std::make_pair(YMatePair::RR,YColor(0.9, 0.9, 0.9, 0.8))); //normal
    colorMap.insert(std::make_pair(YMatePair::PM,YColor(0.8, 0.8, 0.0, 0.8))); //partial match
    colorMap.insert(std::make_pair(YMatePair::FR,YColor(0.8, 0.0, 0.0, 0.8))); //inversion
    colorMap.insert(std::make_pair(YMatePair::RF,YColor(0.0, 0.8, 0.0, 0.8))); //inversion
    colorMap.insert(std::make_pair(YMatePair::IN,YColor(0.8, 0.25, 0.0, 0.8)));//insertion
    colorMap.insert(std::make_pair(YMatePair::DL,YColor(0.5, 0.0, 0.5, 0.8)));//deletion
    //colorMap.insert(std::make_pair(YMatePair::NF,YColor(0.5, 0.0, 0.5, 0.8)));
}

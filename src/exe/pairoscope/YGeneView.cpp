/*----------------------------------
  $Author$ 
  $Date$
  $Revision$
  $URL$
  ----------------------------------*/

#include "YGeneView.h"
#include <string> //finally getting around to using C++ strings
#include <cstring> //still necessary
#include <cstdio>

YGeneView::YGeneView(cairo_t *cr, YRect initialFrame, const char *refName, unsigned int physicalStart, unsigned int physicalStop, std::vector<YTranscript*> *transcriptVector, bool topTranscript, double fontSize) : YView(cr,initialFrame), transcripts(transcriptVector), refName(NULL), physicalStart(physicalStart), physicalStop(physicalStop) {
    this->plotArea = initialFrame;
    int length = strlen(refName);
    this->refName = new char[length + 1]; //create space for a copy of the refName
    strcpy(this->refName, refName);
    this->fontSize = fontSize;
    this->topTranscript = topTranscript;
    
}


YGeneView::~YGeneView() {
    if(refName) {
        delete[] refName;
    }
}

void YGeneView::draw() {
    //Now draw the axes 
    cairo_save(context);
    cairo_translate(context, plotArea.x, plotArea.y);
    cairo_scale(context, plotArea.width/bounds.width, plotArea.height/bounds.height);
    cairo_rectangle(context, 0.0, plotArea.y,physicalStop-physicalStart, plotArea.height);
    cairo_clip(context);

    //FIXME Find font extents here, convert to local coordinates, then assign transcript sizes

    //Set font up
    cairo_select_font_face(context, "Times", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD); //hardcoding for now
    cairo_set_font_size(context, fontSize);	//hardcoding for now
    cairo_font_extents_t font_extents;
    cairo_font_extents(context,&font_extents);
    double font_height = font_extents.ascent + font_extents.descent;    //try to get maximum font height for spacing. May be imperfect
    fprintf(stderr, "font height: %f\n",font_height); 
    double dummy = 0;
    cairo_device_to_user_distance(context, &dummy, &font_height);
    fprintf(stderr, "scaled font height: %f\n",font_height); 

    std::vector<YTranscript*> *transcriptsToGraph;
    if(topTranscript) {
        transcriptsToGraph = new std::vector<YTranscript*>;
        YTranscript *selectedTranscript = NULL;
        for(size_t j = 0; j < transcripts->size(); ++j) {
            YTranscript *candidateTranscript = (*transcripts)[j];

            if(selectedTranscript == NULL) {
                selectedTranscript = candidateTranscript;
            }
            else {
                if(transcriptStatusPriority(selectedTranscript->status) > transcriptStatusPriority(candidateTranscript->status)) {
                    selectedTranscript = candidateTranscript;
                }
                else {
                    if(transcriptStatusPriority(selectedTranscript->status) == transcriptStatusPriority(candidateTranscript->status) && selectedTranscript->length < candidateTranscript->length) {
                        selectedTranscript = candidateTranscript;
                    }
                }
            }
        }
        if(selectedTranscript) {
            transcriptsToGraph->push_back(selectedTranscript);
        }
    }
    else {
        transcriptsToGraph = transcripts;
    }

    int numberTranscripts = transcriptsToGraph->size();
    fprintf(stderr,"#Transcripts = %i\n",numberTranscripts);

    if(numberTranscripts==0) {
        double yTextLocation = plotArea.height / 2 + font_height;
        cairo_move_to(context, 0.0 ,yTextLocation);
        cairo_save(context);
        cairo_identity_matrix(context);	//scale to actual size of document
        cairo_set_source_rgb(context, 0, 0, 0);  //draw in black
        cairo_show_text(context, "No exons in this region");
        cairo_restore(context);
    }
    else {


        double transcriptHeight = (plotArea.height - numberTranscripts *8.0 - 3.0) / numberTranscripts; //default 3 pixel spacing
        fprintf(stderr,"#Transcript height = %f\n",transcriptHeight);

        //iterate over the transcripts and draw them
        unsigned int i;
        for(i = 0; i != transcriptsToGraph->size(); i++) {
            double upperBound = plotArea.y + (3.0 + i*(8.0 + transcriptHeight));
            double transcriptUpperBound = upperBound + font_height + 5.0;
            double linePosition = transcriptUpperBound + (transcriptHeight - font_height)/2;
            fprintf(stderr,"#UB = %f\n",upperBound);
            YTranscript *transcript = (*transcriptsToGraph)[i];
            fprintf(stderr, "Gene: %s\n",transcript->gene);
            unsigned int j;
            YTranscriptSubStructure structure;
            cairo_move_to(context, 0.0 ,upperBound + font_height);
                cairo_save(context);
                cairo_identity_matrix(context);	//scale to actual size of document
                cairo_set_source_rgb(context, 0, 0, 0);  //draw in black
                std::string geneLabel = transcript->gene;
                geneLabel += "(";
                geneLabel += transcript->name;
                geneLabel += ")";
                geneLabel += " Exons ";
                char startExon[150], endExon[150], exonNumber[150];
                snprintf(startExon,sizeof(char)*149,"%d",transcript->orderedStructures[0].ordinal+1);
                snprintf(endExon,sizeof(char)*149,"%d",transcript->orderedStructures[transcript->orderedStructures.size()-1].ordinal+1);
                snprintf(exonNumber,sizeof(char)*149,"%d",transcript->totalNumberOfStructures);
                
                if(transcript->strand == 1) {
                    geneLabel += startExon; 
                    geneLabel += "-";
                    geneLabel += endExon;
                    geneLabel += " out of ";
                    snprintf(exonNumber,sizeof(char)*149,"%d",transcript->totalNumberOfStructures);
                    geneLabel += exonNumber;
                    geneLabel.append(" ->");
                }
                else {
                    geneLabel += endExon;
                    geneLabel += "-";
                    geneLabel += startExon; 
                    geneLabel += " out of ";
                    snprintf(exonNumber,sizeof(char)*149,"%d",transcript->totalNumberOfStructures);
                    geneLabel += exonNumber;
                    geneLabel.insert(0,"<-");
                }
                cairo_show_text(context, geneLabel.c_str());
            cairo_restore(context);
            cairo_move_to(context, 0.0 ,linePosition);
            for(j=0; j != transcript->orderedStructures.size(); j++) {
                structure = transcript->orderedStructures[j];
                //FIXME This doesn't work depending on stranding issues. The line should be drawn if it is the rightmost exon in a transcript.
                if(!(structure.ordinal == 0 && transcript->strand == 1) && !(structure.ordinal == (transcript->totalNumberOfStructures -1) && transcript->strand==-1)) {
                    cairo_line_to(context, (double) structure.position - physicalStart, linePosition);
                    cairo_save(context);
                    cairo_identity_matrix(context);
                    cairo_set_source_rgb(context,0,0,0);
                    double dash = 3.0;
                    cairo_set_dash(context,&dash, 1, 0);
                    cairo_stroke(context);
                    cairo_restore(context);    
                }
                cairo_rectangle(context, (double) structure.position - physicalStart, transcriptUpperBound, (double) structure.length, (transcriptHeight - font_height) );
                cairo_save(context);
                cairo_identity_matrix(context);	//scale to actual size of document
                cairo_set_source_rgb(context, 0.8, 0.8, 0.8);
                cairo_fill_preserve(context);
                cairo_set_source_rgb(context, 0, 0, 0);
                cairo_stroke(context);
                cairo_restore(context);    
                cairo_move_to(context, (double) structure.position - physicalStart + (double) structure.length, linePosition); 
            }
            if(!(structure.ordinal == 0 && transcript->strand==-1) && !(structure.ordinal == (transcript->totalNumberOfStructures-1) && transcript->strand==1)) {
                cairo_line_to(context, physicalStop - physicalStart, linePosition);
                cairo_save(context);
                cairo_identity_matrix(context);
                cairo_set_source_rgb(context,0,0,0);
                double dash = 3.0;
                cairo_set_dash(context,&dash, 1, 0);
                cairo_stroke(context);
                cairo_restore(context);    
            }
        }
    }
    cairo_restore(context);
}
    
void YGeneView::calculateAxes() {
    setBounds(YRect(0,0,physicalStop-physicalStart, bounds.height));
    plotArea = bounds;
}
    
YRect YGeneView::plotAreaInParentCoordinates() {
    //TODO FOR THE LOVE OF CHRIS, MAKE THIS A BASE CLASS ALREADY
    YRect returnPlotArea = this->plotArea;
    YView::rectInParentCoordinates(&returnPlotArea);
    return returnPlotArea;
    
}

bool YGeneView::setPlotAreaInParentCoordinates(YRect newPlotArea) {
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

int YGeneView::transcriptStatusPriority(char *status) {
    //This hierarchy lifted from internal annotation code
   if(strcmp(status,"reviewed") == 0 ) {
       return 1;
   }
   if(strcmp(status,"validated") == 0 ) {
       return 2;
   }
   if(strcmp(status,"provisional") == 0 ) {
       return 3;
   }
   if(strcmp(status,"predicted") == 0 ) {
       return 4;
   }
   if(strcmp(status,"model") == 0 ) {
       return 5;
   }
   if(strcmp(status,"inferred") == 0 ) {
       return 6;
   }
   return 7;
}

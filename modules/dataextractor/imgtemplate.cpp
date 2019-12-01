#include <cmath>
#include "imgtemplate.h"

int rtr::ImgTemplate::cm_to_pix(double cm){
	return std::round(cm * (double)DPI / 2.54);
}



rtr::TestImgTemplate::TestImgTemplate(int dpi){
	DPI = dpi;
	BORDER_WIDTH_PIX = cm_to_pix(0.2);
	MARKER_WHITESPACE_HEIGHT_PIX = cm_to_pix(1.8);
	MARKER_WHITESPACE_WIDTH_PIX = cm_to_pix(1.6);
	MARKER_WIDTH_PIX = cm_to_pix(0.61);
	IMAGE_TYPE = SCANED_IMAGE;
	GEOM_MARKER_POS_Y_PIX = cm_to_pix(10.5);
	MARKER_TO_MARKER_WIDTH_PIX = cm_to_pix(20.2184);
	MARKER_TO_MARKER_HEIGHT_PIX = cm_to_pix(13.818);
	LETTER_WIDTH_PIX = cm_to_pix(0.05);
	
}


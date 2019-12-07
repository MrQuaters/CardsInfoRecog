#include <cmath>
#include "imgtemplate.h"

int rtr::ImgTemplate::cm_to_pix(double cm){
	return std::round(cm * (double)DPI / 2.54);
}

int cm_to_pix_s(double cm) {
	return std::round(cm * 250.0 / 2.54);
}


rtr::TestImgTemplate::TestImgTemplate(int dpi){
	DPI = dpi;
	BORDER_WIDTH_PIX = cm_to_pix(0.2);
	MARKER_WHITESPACE_HEIGHT_PIX = cm_to_pix(1.8);
	MARKER_WHITESPACE_WIDTH_PIX = cm_to_pix(1.6);
	MARKER_WIDTH_PIX = cm_to_pix(0.61);
	IMAGE_TYPE = SCANED_IMAGE;
	LETTER_WIDTH_PIX = cm_to_pix(0.05);
	GEOM_MARKER_POS_Y_PIX = cm_to_pix(10.5);
	FREE_ZONE_PIX = cm_to_pix_s(0.28);
	MARKER_TO_MARKER_WIDTH_PIX = cm_to_pix(20.2184);
	MARKER_TO_MARKER_HEIGHT_PIX = cm_to_pix(13.818);
	MARKER_TO_MARKER_HEIGHT_PIX_TO_250dpi = cm_to_pix_s(13.818);
	MARKER_TO_MARKER_WIDTH_PIX_TO_250dpi = cm_to_pix_s(20.2184);
	MARKER_DOT_MAX_RADIUS_PIX = 10;
	setted_to_250 = (DPI > 250) ? true : false;
	d_data = {//change for adaptive count in normal class
		data_for_detect(cm_to_pix_s(3.10),cm_to_pix_s(1.76), 26, cm_to_pix_s(0.55), cm_to_pix_s(0.71),cm_to_pix_s(0.12), IS_NUMBERS, "fam", cm_to_pix_s(0.71)/3),
		data_for_detect(cm_to_pix_s(3.10),cm_to_pix_s(2.64), 26, cm_to_pix_s(0.55), cm_to_pix_s(0.71),cm_to_pix_s(0.12), IS_NUMBERS, "nam", cm_to_pix_s(0.71) / 3),
		data_for_detect(cm_to_pix_s(3.10),cm_to_pix_s(4.39), 11, cm_to_pix_s(0.55), cm_to_pix_s(0.71),cm_to_pix_s(0.12), IS_NUMBERS, "TELEPHON", cm_to_pix_s(0.71) / 3),
		data_for_detect(cm_to_pix_s(3.10),cm_to_pix_s(3.5108), 26, cm_to_pix_s(0.55), cm_to_pix_s(0.71),cm_to_pix_s(0.12), IS_TEXT, "otc", cm_to_pix_s(0.71) / 3),
		data_for_detect(cm_to_pix_s(3.10),cm_to_pix_s(6.57), 26, cm_to_pix_s(0.55), cm_to_pix_s(0.71),cm_to_pix_s(0.12), IS_TEXT, "obl", cm_to_pix_s(0.71) / 3),
		data_for_detect(cm_to_pix_s(3.10),cm_to_pix_s(7.45), 26, cm_to_pix_s(0.55), cm_to_pix_s(0.71),cm_to_pix_s(0.12), IS_TEXT, "cty", cm_to_pix_s(0.71) / 3),
		data_for_detect(cm_to_pix_s(3.10),cm_to_pix_s(8.33), 26, cm_to_pix_s(0.55), cm_to_pix_s(0.71),cm_to_pix_s(0.12), IS_TEXT, "rayon", cm_to_pix_s(0.71) / 3),
		data_for_detect(cm_to_pix_s(3.10),cm_to_pix_s(9.21), 26, cm_to_pix_s(0.55), cm_to_pix_s(0.71),cm_to_pix_s(0.12), IS_TEXT, "street", cm_to_pix_s(0.71) / 3),
		data_for_detect(cm_to_pix_s(3.10),cm_to_pix_s(10.65), 4, cm_to_pix_s(0.55), cm_to_pix_s(0.71),cm_to_pix_s(0.12), IS_NUMBERS, "house", cm_to_pix_s(0.71) / 3),
		data_for_detect(cm_to_pix_s(6.40),cm_to_pix_s(10.65), 4, cm_to_pix_s(0.55), cm_to_pix_s(0.71),cm_to_pix_s(0.12), IS_NUMBERS, "korp", cm_to_pix_s(0.71) / 3),
		data_for_detect(cm_to_pix_s(9.70),cm_to_pix_s(10.65), 4, cm_to_pix_s(0.55), cm_to_pix_s(0.71),cm_to_pix_s(0.12), IS_NUMBERS, "stre", cm_to_pix_s(0.71) / 3),
		data_for_detect(cm_to_pix_s(13.0),cm_to_pix_s(10.65), 4, cm_to_pix_s(0.55), cm_to_pix_s(0.71),cm_to_pix_s(0.12), IS_NUMBERS, "korp", cm_to_pix_s(0.71) / 3),
		data_for_detect(cm_to_pix_s(16.3),cm_to_pix_s(10.65), 7, cm_to_pix_s(0.55), cm_to_pix_s(0.71),cm_to_pix_s(0.12), IS_NUMBERS, "korp", cm_to_pix_s(0.71) / 3)
	};
}

rtr::data_for_detect::data_for_detect(int DATA_POS_X, int  DATA_POS_Y,int R_CNTS, int X_SIZ, int Y_SIZ, int RECT_MARGI, int DATA_TYP, std::string nm, int pc){
	DATA_POS_X_PIX = DATA_POS_X;
	DATA_POS_Y_PIX = DATA_POS_Y;
	R_PARAMS.R_COUNTS = R_CNTS;
	R_PARAMS.X_SIZE = X_SIZ;
	R_PARAMS.Y_SIZE = Y_SIZ;
	R_PARAMS.RECT_MARGIN = RECT_MARGI;
	DATA_TYPE = DATA_TYP;
	dname = nm;
	R_PARAMS.PIXEL_CONCENTRATE_THRESHOLD = pc;
}

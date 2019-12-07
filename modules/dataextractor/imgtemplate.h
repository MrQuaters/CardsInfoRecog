#include <string>
#include <vector>
#pragma once
namespace rtr{

constexpr int SCANED_IMAGE = 0;
constexpr int PHOTO_IMAGE = 1;

constexpr int IS_NUMBERS = 0;
constexpr int IS_TEXT = 1;
constexpr int IS_MIXED = 2;

struct data_for_detect {
	int DATA_POS_X_PIX;//pos x
	int DATA_POS_Y_PIX;//pos y
	struct r_params {
		int X_SIZE, Y_SIZE; // letter window params in pix
		int RECT_MARGIN;//margin between letters
		int R_COUNTS;// letters count
		int PIXEL_CONCENTRATE_THRESHOLD;// minEEp(xy) for check if box not empty
	} R_PARAMS;
	int DATA_TYPE;//data type need for next proccessing with neural network
	std::string dname;
	data_for_detect(int, int,  int, int, int, int, int, std::string, int);//fast set all params
};


class ImgTemplate {
protected:
	int cm_to_pix(double cm);
public:	
	int DPI;
	int MARKER_TO_MARKER_WIDTH_PIX;//PIX BETWEEN 2 CORNERS MARKERS needed for final geom transform
	int MARKER_TO_MARKER_HEIGHT_PIX;
	int MARKER_TO_MARKER_WIDTH_PIX_TO_250dpi;//PIX BETWEEN 2 CORNERS MARKERS needed for final geom transform bormaliseed for better perfomance
	int MARKER_TO_MARKER_HEIGHT_PIX_TO_250dpi;
	bool setted_to_250;
	int BORDER_WIDTH_PIX;// not used
	int MARKER_WHITESPACE_WIDTH_PIX;//white window where marker placed... no text in area
	int MARKER_WHITESPACE_HEIGHT_PIX;
	int MARKER_WIDTH_PIX;//marker width in pix
	int GEOM_MARKER_POS_Y_PIX; // center of geometrical marker pos in pix
	int IMAGE_TYPE;//type of image not used
	int LETTER_WIDTH_PIX;//font size not used
	int FREE_ZONE_PIX; //min length y acsiss between 2 data to recognize
	int MARKER_DOT_MAX_RADIUS_PIX; //if dots as d_data borders extractor can filter it if not need, set to -1
	std::vector<data_for_detect> d_data;//data to extrarct
};


//class for test with constant params
class TestImgTemplate : public ImgTemplate {
public:
	TestImgTemplate(int dpi);
};
}
#pragma once
namespace rtr{

constexpr int SCANED_IMAGE = 0;
constexpr int PHOTO_IMAGE = 1;

class ImgTemplate {
protected:
	int cm_to_pix(double cm);
public:	
	int DPI;
	int BORDER_WIDTH_PIX;
	int MARKER_WHITESPACE_WIDTH_PIX;
	int MARKER_WHITESPACE_HEIGHT_PIX;
	int MARKER_WIDTH_PIX;
	int GEOM_MARKER_POS_Y_PIX; // center of geometrical marker pos in pix
	int IMAGE_TYPE;
};


//class for test with constant params
class TestImgTemplate : public ImgTemplate {
public:
	TestImgTemplate();
};
}
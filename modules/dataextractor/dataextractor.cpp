#include "dataextractor.h"
namespace rtr{

void rtr::DataExtractor::_gateway(cv::Mat& mt){
	if (mt.channels() == 3) cv::cvtColor(mt, mt, cv::COLOR_BGR2GRAY);
}

DataExtractor::DataExtractor(const ImgTemplate* t) noexcept {
	imgparams = *t;
}


void DataExtractor::_binarise(cv::Mat& t){
	cv::GaussianBlur(t, t, cv::Size(5, 5), 0);
	cv::threshold(t, t, 0, 255, cv::THRESH_BINARY|cv::THRESH_OTSU);
}

void DataExtractor::_geom_restore(cv::Mat& g){
	int ctl, ctr;
	int gt = imgparams.GEOM_MARKER_POS_Y_PIX - imgparams.MARKER_WHITESPACE_HEIGHT_PIX / 2;
	if (gt < 0) gt = 0;
	int gt_ = imgparams.GEOM_MARKER_POS_Y_PIX + imgparams.MARKER_WHITESPACE_HEIGHT_PIX / 2;
	if (gt_ > g.rows - 1) gt_ = g.rows-1;

	cv::Mat dmat = g(cv::Rect2i(cv::Point2i(0, gt), cv::Point2i(imgparams.MARKER_WHITESPACE_WIDTH_PIX, gt_)));
	auto kl = _mfinder(dmat, &ctl);

	int xt = g.cols - 1 - imgparams.MARKER_WHITESPACE_WIDTH_PIX;
	if (xt < 0) xt = 0;
	gt = g.rows - imgparams.GEOM_MARKER_POS_Y_PIX - imgparams.MARKER_WHITESPACE_HEIGHT_PIX / 2;
	if (gt < 0) gt = 0;
	gt_ = g.rows - imgparams.GEOM_MARKER_POS_Y_PIX + imgparams.MARKER_WHITESPACE_HEIGHT_PIX / 2;
	if (gt_ < 0) gt_ = 0;

	dmat = g(cv::Rect2i(cv::Point2i(xt , gt), cv::Point2i(g.cols - 1, gt_)));
	auto kr = _mfinder(dmat, &ctr);
	if (ctr > ctl) cv::rotate(g, g, cv::ROTATE_180);
}


_corners DataExtractor::_corner_marker_pos_detector(const cv::Mat& g){
	_corners crn;

	cv::Mat dmat = g(cv::Rect2i(cv::Point2i(0,0), cv::Point2i(imgparams.MARKER_WHITESPACE_WIDTH_PIX, imgparams.MARKER_WHITESPACE_HEIGHT_PIX)));
	auto k = _mfinder(dmat);
	crn.lt = k;

	dmat = g(cv::Rect2i(cv::Point2i(0, g.rows - 1 - imgparams.MARKER_WHITESPACE_HEIGHT_PIX), cv::Point2i(imgparams.MARKER_WHITESPACE_WIDTH_PIX, g.rows-1)));
	k = _mfinder(dmat);
	k.y += g.rows - 1 - imgparams.MARKER_WHITESPACE_HEIGHT_PIX;
	crn.lb = k;

	dmat = g(cv::Rect2i(cv::Point2i(g.cols - 1 - imgparams.MARKER_WHITESPACE_WIDTH_PIX, 0), cv::Point2i(g.cols - 1, imgparams.MARKER_WHITESPACE_HEIGHT_PIX)));
	k = _mfinder(dmat);
	k.x += g.cols - 1 - imgparams.MARKER_WHITESPACE_WIDTH_PIX;
	crn.rt = k;

	dmat = g(cv::Rect2i(cv::Point2i(g.cols - 1 - imgparams.MARKER_WHITESPACE_WIDTH_PIX, g.rows - 1 - imgparams.MARKER_WHITESPACE_HEIGHT_PIX), cv::Point2i(g.cols - 1, g.rows - 1)));
	k = _mfinder(dmat);
	k.x += g.cols - 1 - imgparams.MARKER_WHITESPACE_WIDTH_PIX;
	k.y += g.rows - 1 - imgparams.MARKER_WHITESPACE_HEIGHT_PIX;
	crn.rb = k;

	return crn;
}



cv::Point2i DataExtractor::_mfinder(const cv::Mat& k, int* ct) {
	cv::Point2i rpt;

	auto sze = imgparams.MARKER_WIDTH_PIX;
	if (sze % 2 == 0) sze++; //make marker width odd

	int bpy = sze / 2; //marker window center

	int *rmt = new int[(size_t)(k.rows) * k.cols];
	for (int i = 0; i < k.rows * k.cols; ++i) rmt[i] = 0;

	int ctrb = 0;
	for (int i = 0; i < sze; ++i) {
		for (int j = 0; j < sze; ++j) {
			if (k.at<uchar>(j, i) == 0) ++ctrb; //start section EEp(x,y)
		}
	}

	//counting values for all pixels in area sze*sze for each pixel in xe[bpy; cols-bpy), ye[bpy, rows -bpy)
	for (int i = sze; i < k.rows;i++) {
		int bpx = sze / 2;
		int ctr = ctrb;
		
		for (int j = sze;j < k.cols; ++j) {
			 
			int bb = 0, be = 0;

			for (int ct_ = i - sze; ct_ < i; ++ct_) {
				if (k.at<uchar>(ct_, j) == 0) ++be;
				if (k.at<uchar>(ct_, j - sze) == 0) ++bb;
			}
			ctr = ctr - bb + be;
			rmt[bpy*k.cols + bpx] = ctr;
			++bpx;
		}

		int yb = 0, ye = 0;

		for (int ct_ = 0; ct_ < sze; ++ct_) {
			if (k.at<uchar>(i-sze, ct_) == 0) ++yb;
			if (k.at<uchar>(i, ct_) == 0) ++ye;
		}
		ctrb = ctrb - yb + ye;
		++bpy;
	}
	//finding max val
	int max = 0;
	for (int i = 0; i < k.rows; ++i) {
		for (int j = 0; j < k.cols; ++j) {
			if (rmt[i * k.cols + j] > max) {
				max = rmt[i * k.cols + j];
			}
		}	
	}

	//counting center masses
	int spx = 0, spy = 0, maxescount = 0;
	for (int i = 0; i < k.rows; ++i) {
		for (int j = 0; j < k.cols; ++j) {
			if (rmt[i * k.cols + j] == max) {
				spx += j;
				spy += i;
				++maxescount;
			}
		}
	}
	rpt.x = spx/maxescount;
	rpt.y = spy/maxescount;

	if (ct != nullptr) *ct = max;

	delete[] rmt;//free resources

	return rpt;
}

void DataExtractor::_geom_wrap_prespective(cv::Mat& mt, _corners cn){
	std::vector<cv::Point2f> gtp = {
		cv::Point2f(0,0),
		cv::Point2f(imgparams.MARKER_TO_MARKER_WIDTH_PIX, 0),
		cv::Point2f(imgparams.MARKER_TO_MARKER_WIDTH_PIX, imgparams.MARKER_TO_MARKER_HEIGHT_PIX),
		cv::Point2f(0, imgparams.MARKER_TO_MARKER_HEIGHT_PIX)
	};
	std::vector<cv::Point2f> gtn = {
		cv::Point2f(cn.lt.x -imgparams.MARKER_WIDTH_PIX/2, cn.lt.y - imgparams.MARKER_WIDTH_PIX / 2),
		cv::Point2f(cn.rt.x + imgparams.MARKER_WIDTH_PIX / 2, cn.rt.y - imgparams.MARKER_WIDTH_PIX / 2),
		cv::Point2f(cn.rb.x + imgparams.MARKER_WIDTH_PIX / 2, cn.rb.y + imgparams.MARKER_WIDTH_PIX / 2),
		cv::Point2f(cn.lb.x - imgparams.MARKER_WIDTH_PIX / 2, cn.lb.y + imgparams.MARKER_WIDTH_PIX / 2)
	};
	auto hg = cv::findHomography(gtn, gtp);
	cv::warpPerspective(mt, mt, hg, cv::Size2i(imgparams.MARKER_TO_MARKER_WIDTH_PIX, imgparams.MARKER_TO_MARKER_HEIGHT_PIX));
}

void DataExtractor::_final_binarisation(cv::Mat& t){
	cv::adaptiveThreshold(t, t, 255, cv::THRESH_BINARY, cv::ADAPTIVE_THRESH_GAUSSIAN_C, 3, 0);
}


static int gdsf = 0;

std::vector<extrdata> rtr::DataExtractor::data_extract(const cv::Mat& t){
	cv::Mat vm = t;
	_gateway(vm); // using gateway to prepare image
	_binarise(vm);//binarisation for finding markers
	auto r = _corner_marker_pos_detector(vm);//finding corner markers
	_geom_wrap_prespective(vm, r);//wraping perspective
	_geom_restore(vm);// restoring position in space
	cv::imwrite("afd" + std::to_string(gdsf++)+".jpg", vm);
	return std::vector<extrdata>();
}



}
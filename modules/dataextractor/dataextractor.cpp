#include "dataextractor.h"
namespace rtr{

void rtr::DataExtractor::_gateway(cv::Mat& mt){
	if (mt.channels() == 3) cv::cvtColor(mt, mt, cv::COLOR_BGR2GRAY);
}

DataExtractor::DataExtractor(const ImgTemplate* t) noexcept {
	imgparams = *t;
}

void _miniotsu(cv::Mat& t) {
	cv::threshold(t, t, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
}


void DataExtractor::_binarise(cv::Mat& t){ //binirasing by OTSU thresholding
	cv::GaussianBlur(t, t, cv::Size(5, 5), 0);
	int stepx = t.cols / 2;
	int stepy = t.rows / 2;
	cv::Mat r;
	t.copyTo(r);
	for (int i = 1; i <= 2; ++i) {
		int clx = (i - 1) * stepx;
		int clx_ = i * stepx;
		if (i == 2) clx_ = t.cols;
		for (int j = 1; j <= 2; ++j) {
			int cly = (j - 1) * stepy;
			int cly_ = j * stepy;
			if (j == 2) cly_ = t.rows;
			auto f = t(cv::Rect(cv::Point(clx, cly), cv::Point(clx_, cly_)));
			_miniotsu(f);
			int cx = clx;
			for (int k = 0; k < f.cols; ++k) {
				int cp = cly;
				for (int o = 0; o < f.rows; ++o) {
					r.at<uchar>(cp++, cx) = f.at<uchar>(o, k);
				}
				++cx;
			}
		}
	}
	r.copyTo(t);
}

bool DataExtractor::_geom_restore(cv::Mat& g){ //restoring geometry if no geom marker in setted position, should rotate image
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
	return (ctr > ctl);
}


_corners DataExtractor::_corner_marker_pos_detector(const cv::Mat& g){
	_corners crn;

	cv::Mat dmat = g(cv::Rect2i(cv::Point2i(0,0), cv::Point2i(imgparams.MARKER_WHITESPACE_WIDTH_PIX, imgparams.MARKER_WHITESPACE_HEIGHT_PIX)));
	auto k = _mfinder(dmat);
	crn.lt = k;//left top corner

	dmat = g(cv::Rect2i(cv::Point2i(0, g.rows - 1 - imgparams.MARKER_WHITESPACE_HEIGHT_PIX), cv::Point2i(imgparams.MARKER_WHITESPACE_WIDTH_PIX, g.rows-1)));
	k = _mfinder(dmat);
	k.y += g.rows - 1 - imgparams.MARKER_WHITESPACE_HEIGHT_PIX;
	crn.lb = k;//left bot

	dmat = g(cv::Rect2i(cv::Point2i(g.cols - 1 - imgparams.MARKER_WHITESPACE_WIDTH_PIX, 0), cv::Point2i(g.cols - 1, imgparams.MARKER_WHITESPACE_HEIGHT_PIX)));
	k = _mfinder(dmat);
	k.x += g.cols - 1 - imgparams.MARKER_WHITESPACE_WIDTH_PIX;
	crn.rt = k;//right top corner

	dmat = g(cv::Rect2i(cv::Point2i(g.cols - 1 - imgparams.MARKER_WHITESPACE_WIDTH_PIX, g.rows - 1 - imgparams.MARKER_WHITESPACE_HEIGHT_PIX), cv::Point2i(g.cols - 1, g.rows - 1)));
	k = _mfinder(dmat);
	k.x += g.cols - 1 - imgparams.MARKER_WHITESPACE_WIDTH_PIX;
	k.y += g.rows - 1 - imgparams.MARKER_WHITESPACE_HEIGHT_PIX;
	crn.rb = k;//left bot corner

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

void DataExtractor::_geom_wrap_prespective(cv::Mat& mt, _corners cn){//wraping perspective to restore geometry
	std::vector<cv::Point2f> gtp = {
		cv::Point2f((float)0,(float)0),
		cv::Point2f((float)(imgparams.MARKER_TO_MARKER_WIDTH_PIX), (float)0),
		cv::Point2f((float)(imgparams.MARKER_TO_MARKER_WIDTH_PIX), (float)(imgparams.MARKER_TO_MARKER_HEIGHT_PIX)),
		cv::Point2f((float)0, (float)(imgparams.MARKER_TO_MARKER_HEIGHT_PIX))
	};
	std::vector<cv::Point2f> gtn = {
		cv::Point2f((float)(cn.lt.x -imgparams.MARKER_WIDTH_PIX/2), (float)(cn.lt.y - imgparams.MARKER_WIDTH_PIX / 2)),
		cv::Point2f((float)(cn.rt.x + imgparams.MARKER_WIDTH_PIX / 2), (float)(cn.rt.y - imgparams.MARKER_WIDTH_PIX / 2)),
		cv::Point2f((float)(cn.rb.x + imgparams.MARKER_WIDTH_PIX / 2), (float)(cn.rb.y + imgparams.MARKER_WIDTH_PIX / 2)),
		cv::Point2f((float)(cn.lb.x - imgparams.MARKER_WIDTH_PIX / 2), (float)(cn.lb.y + imgparams.MARKER_WIDTH_PIX / 2))
	};
	auto hg = cv::findHomography(gtn, gtp);
	cv::warpPerspective(mt, mt, hg, cv::Size2i(imgparams.MARKER_TO_MARKER_WIDTH_PIX, imgparams.MARKER_TO_MARKER_HEIGHT_PIX));
}

void DataExtractor::_resize(cv::Mat& a){
	cv::Size r;
	r.width = imgparams.MARKER_TO_MARKER_WIDTH_PIX_TO_250dpi;
	r.height = imgparams.MARKER_TO_MARKER_HEIGHT_PIX_TO_250dpi;
	cv::resize(a, a, r);
}

void DataExtractor::_final_binarisation(cv::Mat& t){
	cv::adaptiveThreshold(t, t, 255, cv::THRESH_BINARY, cv::ADAPTIVE_THRESH_GAUSSIAN_C, 3, 0);
}


int _pointfnd(int* g, int ct, int thresh, int sthresh = -1) {
	int xmaxval = 0;
	int xmax = 0;
	unsigned long long summ = 0;
	for (int j = 0; j < ct; ++j) {
		summ += g[j];
		if (g[j] > xmaxval) {
			xmaxval = g[j];
			xmax = j;
		}
	}
	if (sthresh != -1) {
		if (sthresh >= summ) return -1;
	}
	summ /= 2;

	if (xmaxval < thresh) return -1;
	unsigned long long summ_ = 0;
	
	for (int j = 0; j < ct; ++j) {
		summ_ += g[j];
		if (summ_ >= summ) return j;
	}

	return -1;
}


extrdata DataExtractor::_data_extract(cv::Mat& mt, const data_for_detect& d){
	cv::Rect2i r;
	
	r.x = d.DATA_POS_X_PIX - imgparams.FREE_ZONE_PIX / 2; 
	r.y = d.DATA_POS_Y_PIX - imgparams.FREE_ZONE_PIX / 2;
	r.width = d.R_PARAMS.R_COUNTS * d.R_PARAMS.X_SIZE + (d.R_PARAMS.R_COUNTS-1) * d.R_PARAMS.RECT_MARGIN + imgparams.FREE_ZONE_PIX;
	r.height = d.R_PARAMS.Y_SIZE + imgparams.FREE_ZONE_PIX/2;
	if (r.x + r.width > mt.cols) r.width = mt.cols - r.x-1;
	
#ifdef MODULAR_TEST_DATAEXTRACTOR
	cv::rectangle(modular_test_matrix, r, cv::Scalar(0), 1);
#endif // MODULAR_TEST_DATAEXTRACTOR

	cv::Mat mtn = mt(r);//cutting image in givven place, with +- FREE_SPACE/2 to proccess errors of geometrical transformation

	int* g = new int[mtn.cols]; // counting black pixels for each coll
	int xgmval = 0;
	for (int i = 0; i < mtn.cols; ++i) { 
		int ctn = 0;
		for (int j = 0; j < mtn.rows; ++j) {
			if (mtn.at<uchar>(j, i) == 0) { 
				ctn++;
			}
		}
		xgmval = (xgmval > ctn) ? xgmval : ctn;
		g[i] = ctn; 
	}
	if (xgmval < d.R_PARAMS.PIXEL_CONCENTRATE_THRESHOLD) xgmval = d.R_PARAMS.PIXEL_CONCENTRATE_THRESHOLD;
	//finding center position of letters
	int nxtpt = 0;
	int ac = (imgparams.FREE_ZONE_PIX - d.R_PARAMS.RECT_MARGIN) / 2;
	int bpt = 0;
	std::vector<int> xpicslist(d.R_PARAMS.R_COUNTS, -1);
	
	for (int i = 0; i < d.R_PARAMS.R_COUNTS; ++i) {//foreach lettter
		int rng = d.R_PARAMS.X_SIZE + d.R_PARAMS.RECT_MARGIN + ac;
		nxtpt = bpt + rng;//next step calc by last step
		if (nxtpt > mtn.cols) rng = mtn.cols - bpt;
		int rp = _pointfnd(&g[bpt], rng, xgmval / 8);
		if (rp != -1) {
			xpicslist[i] = bpt + rp;
			bpt = xpicslist[i] + (d.R_PARAMS.X_SIZE + d.R_PARAMS.RECT_MARGIN) / 2; 
		}
		else { 
			bpt = nxtpt; 
		}
		ac = 0;
	}

	extrdata ed;

	int* gy = new int[mtn.rows];

	for (int i = 0; i < xpicslist.size(); ++i) {

		if (xpicslist[i] == -1) {
			ed.imgs.push_back(cv::Mat());
			continue;
		}

		for (int j = 0; j < mtn.rows; ++j) {
			gy[j] = 0;
		}
		int endp = xpicslist[i] + d.R_PARAMS.X_SIZE / 2;
		int begp = xpicslist[i] - d.R_PARAMS.X_SIZE / 2;
		if (begp < 0) { 
			endp -= begp;
			begp = 0; 
		}
		
		if (endp > mtn.cols ) endp = mtn.cols;

		int maxvy = 0;
		//calculating local y absciss hist
		for (int j = 0; j < mtn.rows; ++j) {
			for (int k = begp; k < endp; ++k) {
				if (mtn.at<uchar>(j, k) == 0) ++gy[j];
			}
			if (gy[j] > maxvy) maxvy = gy[j];
		}

		int cp = _pointfnd(gy, mtn.rows, maxvy / 8);//finding y masscenter to compensate y aditional space
		if (cp == -1) {
			ed.imgs.push_back(cv::Mat());
			continue;
		}

		
		int yb = cp - d.R_PARAMS.Y_SIZE / 2;
		int ye = cp + d.R_PARAMS.Y_SIZE / 2;
		if (yb < 0) {
			ye -= yb;
			yb = 0;
		}
		if (ye > mtn.rows) {
			yb -= ye - mtn.rows;
			ye = mtn.rows;
			if (yb < 0) yb = 0;
		}

#ifdef MODULAR_TEST_DATAEXTRACTOR
		cv::line(modular_test_matrix, cv::Point(xpicslist[i] + r.x, r.y + yb), cv::Point(r.x + xpicslist[i], r.y + ye), cv::Scalar(0), 1);
		cv::line(modular_test_matrix, cv::Point(r.x+begp, r.y + cp), cv::Point(r.x +endp, r.y + cp), cv::Scalar(0), 1);
#endif // MODULAR_TEST_DATAEXTRACTOR

		auto f = mtn(cv::Rect(cv::Point(begp, yb), cv::Point(endp, ye)));
		
		cv::Mat r; f.copyTo(r);
		ed.imgs.push_back(r);
	}

	ed.dtype = d.DATA_TYPE;
	ed.name = d.dname;

	delete[] gy;
	delete[] g;
	return ed;
}



std::vector<extrdata> rtr::DataExtractor::data_extract(cv::Mat& t, bool dvt){
	cv::Mat vm = t;
	_gateway(vm); // using gateway to prepare image
	_binarise(vm);//binarisation for finding markers
	auto r = _corner_marker_pos_detector(vm);//finding corner markers
	_geom_wrap_prespective(vm, r);//wraping perspective
	if (dvt) _geom_wrap_prespective(t, r);
	auto gr = _geom_restore(vm);// restoring position in space
	
	if(gr)  cv::rotate(vm, vm, cv::ROTATE_180);
	if(gr&&dvt) cv::rotate(t, t, cv::ROTATE_180);
	

	if (imgparams.setted_to_250) _resize(vm);
	if (imgparams.setted_to_250 && dvt) _resize(t);

	if (imgparams.MARKER_DOT_MAX_RADIUS_PIX != -1) _dot_deleter(vm, imgparams.MARKER_DOT_MAX_RADIUS_PIX);

#ifdef MODULAR_TEST_DATAEXTRACTOR
vm.copyTo(modular_test_matrix);
#endif // MODULAR_TEST_DATAEXTRACTOR

	std::vector<extrdata> ed;

	for (int i = 0; i < imgparams.d_data.size(); ++i) {
		auto r = _data_extract(vm, imgparams.d_data[i]);
		ed.push_back(r);
	}
#ifdef MODULAR_TEST_DATAEXTRACTOR
	modular_test_callback();
#endif // MODULAR_TEST_DATAEXTRACTOR
	
	return ed;
}



void _dot_deleter(cv::Mat& a, int dotsize) {
	cv::Mat g;
	a.copyTo(g);
	for (int j = 0; j < a.rows - dotsize - 2; ++j) {
		for (int i = 0; i < a.cols - dotsize - 2;) {
			int t = 0;
			int pos = i;
			for (int k = 0; t == 0 && k < dotsize + 2; ++k) {
				t = 255 - g.at<uchar>(j + k, i);
				t += 255 - g.at<uchar>(j + k, i + 1 + dotsize);
			}
			for (int k = 0; t == 0 && k < dotsize + 2; ++k) {
				t = 255 - g.at<uchar>(j , i +k);
				t += 255 - g.at<uchar>(j +1+dotsize, i +k);
				pos = i + k;
			}
			bool q = false;
			if (t == 0) {
				q = true;
				for (int o = 0; o < dotsize; ++o)
					for (int z = 0; z < dotsize; ++z)
						a.at<uchar>(j + 1 + o, i + 1 + z) = 255;
			}
			if (q) i = --pos;
			else i = ++pos;
		}
	}

}

}
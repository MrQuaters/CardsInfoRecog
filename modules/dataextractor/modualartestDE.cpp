#include "dataextractor.h"
#ifdef MODULAR_TEST_DATAEXTRACTOR


#ifdef DATAEXTRACTOR_MAIN
#include <ctime>
static cv::Mat smat;
void rtr::DataExtractor::modular_test_callback() {
	smat = modular_test_matrix;
}

int main() {

	rtr::TestImgTemplate g(300);
	rtr::DataExtractor u(&g);

	for (int i = 1; i < 10; ++i) {
		auto r = cv::imread("imforproc/" + std::to_string(i) + ".jpg", cv::IMREAD_GRAYSCALE);
		auto t1 = clock() / (float)CLOCKS_PER_SEC; 
		auto x = u.data_extract(r, true);
		auto t2 = clock() / (float)CLOCKS_PER_SEC - t1;
		int a;
		auto ts = cv::getTextSize("E", cv::FONT_HERSHEY_SIMPLEX, 2, 2, &a);
		auto txt = "alhorythm exec time = " + std::to_string(t2) + "s";
		auto ts2 = cv::getTextSize(txt, cv::FONT_HERSHEY_SIMPLEX, 1, 2, &a);
		int p = x.size();
		int cols_max = (r.cols > smat.cols) ? r.cols : smat.cols;
		cols_max = (cols_max > ts2.width) ? cols_max : ts2.width;
		int rows_size = r.rows + smat.rows + ts2.height + 10;
		
		std::vector<int> rmax(p, 0);
		for (int u = 0; u < p; ++u) {
			int cols_cur = 0;
			int rws = 0;
			for (int g = 0; g < x[u].imgs.size(); ++g) {
				int rw = 0;
				int cl = 0;
				if (x[u].imgs[g].empty()) {
					rw = ts.height;
					cl = ts.width;
				}
				else {
					rw = x[u].imgs[g].rows;
					cl = x[u].imgs[g].cols;
				}
				if (rw > rws) rws = rw;
				cols_cur += cl + 2;
			}

			if (cols_max < cols_cur) cols_max = cols_cur;
			rows_size += rws +5;
			rmax[u] = rws ;
		}
		
		cv::Mat rmat(cv::Size(cols_max, rows_size), CV_8UC1, cv::Scalar(160));
		int bs = 0;
		for (int j = 0; j < r.rows; ++j) {
			for (int k = 0; k < r.cols; ++k) {
				rmat.at<uchar>(j, k) = r.at<uchar>(j, k);
			}
		}
		bs += r.rows + 3;

		for (int j = 0; j < smat.rows; ++j) {
			for (int k = 0; k < smat.cols; ++k) {
				rmat.at<uchar>(j + bs, k) = smat.at<uchar>(j, k);
			}
		}

		bs += smat.rows + 3;

		for (int u = 0; u < p; ++u) {
			int xhook = 0;
			for (int g = 0; g < x[u].imgs.size(); ++g) {
				if (x[u].imgs[g].empty()) {
					cv::putText(rmat, "E", cv::Point2i(xhook, bs + ts.height), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(255), 2);
					xhook += ts.width + 2;
					continue;
				}

				for (int j = 0; j < x[u].imgs[g].rows; ++j) {
					for (int k = 0; k < x[u].imgs[g].cols; ++k) {
						rmat.at<uchar>(j + bs , k + xhook) = x[u].imgs[g].at<uchar>(j, k);
					}
				}
				xhook += x[u].imgs[g].cols + 2;
			}
			bs += rmax[u] + 3;
		}

		cv::putText(rmat, txt, cv::Point2i(0, bs + ts2.height), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2);

		cv::imwrite("test/dataextactor/loghigh_" + std::to_string(i) + ".jpg", rmat);
	}

}

#endif 

#ifndef DATAEXTRACTOR_MAIN	
static int fgg = 0;
void rtr::DataExtractor::modular_test_callback() {
	cv::imwrite("test/dataextactor/loglow_" + std::to_string(fgg++)+".jpg", modular_test_matrix);
}

#endif // DATAEXTRACTOR_MAIN
#endif
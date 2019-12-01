#include "modules/dataextractor/dataextractor.h"

int main() {
	rtr::TestImgTemplate g(250);
	rtr::DataExtractor u(&g);
	
	for (int i = 1; i < 4; ++i) {
		auto r = cv::imread("imforproc/"+std::to_string(i)+"_300.jpg");
	    //auto r = cv::imread("imforproc/1_300.jpg");
		u.data_extract(r);
	}

	int a = 0;
	return 0;
}
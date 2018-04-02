//  colorTransfer

#include <iostream>
#include <opencv2/opencv.hpp>


cv::Vec3d BGR2lab(cv::Vec3d bgr) {
    double L = 0.0;
    double M = 0.0;
    double S = 0.0;
    L = 0.3811 * bgr[2] + 0.5783 * bgr[1] + 0.0402 * bgr[0];
    M = 0.1967 * bgr[2] + 0.7244 * bgr[1] + 0.0782 * bgr[0];
    S = 0.0241 * bgr[2] + 0.1288 * bgr[1] + 0.8444 * bgr[0];
    cv::Vec3d LMS;
    LMS[0] = log(L);
    LMS[1] = log(M);
    LMS[2] = log(S);
    cv::Vec3d lab;
    lab[0] = 1 / sqrt(3) * (LMS[0] + LMS[1] + LMS[2]);
    lab[1] = 1 / sqrt(6) * (LMS[0] + LMS[1] - 2 * LMS[2]);
    lab[2] = 1 / sqrt(2) * (LMS[0] - LMS[1]);
    
    return lab;
}

int main(int argc, const char * argv[]) {
    //Load src_bgr image
    cv::Mat src_bgr;
    src_bgr = cv::imread("src.jpg",CV_LOAD_IMAGE_COLOR);
    if(src_bgr.empty()) {
        std::cout << "error" << std::endl;
        return -1;
    }
    //Load ref_bgr image
    cv::Mat ref_bgr;
    ref_bgr = cv::imread("ref.jpg" ,CV_LOAD_IMAGE_COLOR);
    if(ref_bgr.empty()) {
        std::cout << "error" << std::endl;
        return -1;
    }
   
    //convert uchar -> double
    src_bgr.convertTo(src_bgr, CV_64FC3, 1.0 / 255.0);
    ref_bgr.convertTo(ref_bgr, CV_64FC3, 1.0 / 255.0);
    
    
    //inizsialize src_lab, ref_lab
    cv::Mat src_lab = cv::Mat::zeros(src_bgr.size(), CV_64FC3);
    cv::Mat ref_lab = cv::Mat::zeros(ref_bgr.size(), CV_64FC3);
    
    for (int y = 0; y < src_bgr.rows; y++) {
        for (int x = 0; x < src_bgr.cols; x++) {
            src_lab.at<cv::Vec3d>(y,x) = BGR2lab(src_bgr.at<cv::Vec3d>(y,x));
        }
    }
    
    
    
    
    
//    cv::Mat3f src_bgr3f = cv::Mat3f(src_bgr);
//    cv::Mat3f ref_bgr3f = cv::Mat3f(ref_bgr);
    
    
//    for (int y = 0; y < src_bgr.rows; y++) {
//        for (int x = 0; x < src_bgr.cols; x++) {
//            for(int BGR = 0; BGR < 3; BGR++){
//                std::cout << src_bgr.at<cv::Vec3b>(y,x)[BGR]<< std::endl;
//            }
//        }
//    }
    
    
    cv::imshow("src_bgr", src_bgr);
    cv::imshow("ref_bgr", ref_bgr);
    cv::waitKey(0);
}

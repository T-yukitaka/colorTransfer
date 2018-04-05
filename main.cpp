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
    LMS[0] = L > 1.0e-5 ? std::log10(L) : std::log10(1.0e-5);
    LMS[1] = M > 1.0e-5 ? std::log10(M) : std::log10(1.0e-5);
    LMS[2] = S > 1.0e-5 ? std::log10(S) : std::log10(1.0e-5);
    
    cv::Vec3d lab;
    lab[0] = 1 / sqrt(3) * (LMS[0] + LMS[1] + LMS[2]);
    lab[1] = 1 / sqrt(6) * (LMS[0] + LMS[1] - 2 * LMS[2]);
    lab[2] = 1 / sqrt(2) * (LMS[0] - LMS[1]);
    return lab;
}

cv::Vec3d lab2BGR(cv::Vec3d lab) {
    double L = 0.0;
    double M = 0.0;
    double S = 0.0;
    
    L = sqrt(3) / 3 * lab[0] + sqrt(6) / 6 * lab[1] + sqrt(2) / 2 * lab[2];
    M = sqrt(3) / 3 * lab[0] + sqrt(6) / 6 * lab[1] - sqrt(2) / 2 * lab[2];
    S = sqrt(3) / 3 * lab[0] -2 * sqrt(6) / 6 * lab[1];
    
    cv::Vec3d LMS;
    LMS[0] = L > -5 ? std::pow(10, L) : 1.0e-5;
    LMS[1] = M > -5 ? std::pow(10, M) : 1.0e-5;
    LMS[2] = S > -5 ? std::pow(10, S) : 1.0e-5;
    
    cv::Vec3d bgr;
    bgr[2] =   4.4679 * LMS[0] - 3.5873 * LMS[1] + 0.1193 * LMS[2];
    bgr[1] = - 1.2186 * LMS[0] + 2.3809 * LMS[1] - 0.1624 * LMS[2];
    bgr[0] =   0.0497 * LMS[0] - 0.2439 * LMS[1] + 1.2045 * LMS[2];
    
    return bgr;
}

int main(int argc, const char * argv[]) {
    //Load src_bgr image (uchar)
    cv::Mat src_bgr;
    std::string src;
    std::cout << "入力する画像の名前を入力してください" << std::endl;
    std::cin >> src;
    src_bgr = cv::imread(src + ".jpg",CV_LOAD_IMAGE_COLOR);
    if(src_bgr.empty()) {
        std::cout << "error" << std::endl;
        return -1;
    }
    //Load ref_bgr image (uchar)
    cv::Mat ref_bgr;
    std::string ref;
    std::cout << "反映する画像の名前を入力してください" << std::endl;
    std::cin >> ref;
    ref_bgr = cv::imread(ref + ".jpg" ,CV_LOAD_IMAGE_COLOR);
    if(ref_bgr.empty()) {
        std::cout << "error" << std::endl;
        return -1;
    }
    
    //convert uchar -> double
    src_bgr.convertTo(src_bgr, CV_64FC3, 1.0 / 255.0);
    ref_bgr.convertTo(ref_bgr, CV_64FC3, 1.0 / 255.0);
    
    //inizsialize src_lab, ref_lab (double)
    cv::Mat src_lab = cv::Mat(src_bgr.size(), CV_64FC3);
    cv::Mat ref_lab = cv::Mat(ref_bgr.size(), CV_64FC3);
    
    int src_height = src_bgr.rows;
    int src_width = src_bgr.cols;
    int ref_height = ref_bgr.rows;
    int ref_width = ref_bgr.cols;
    
    //convert BGR to lab (src)
    for (int y = 0; y < src_height; y++) {
        for (int x = 0; x < src_width; x++) {
            src_lab.at<cv::Vec3d>(y,x) = BGR2lab(src_bgr.at<cv::Vec3d>(y,x));
        }
    }
    
    //convert BGR to lab (ref)
    for (int y = 0; y < ref_height; y++) {
        for (int x = 0; x < ref_width; x++) {
            ref_lab.at<cv::Vec3d>(y,x) = BGR2lab(ref_bgr.at<cv::Vec3d>(y,x));
        }
    }
    
    //calculate lab_src_sum, lab_square_src_sum
    double lab_src_sum[3] = {0.0, 0.0, 0.0};
    double lab_square_src_sum[3] = {0.0, 0.0, 0.0};
    double src_buf = 0.0;
    for (int y = 0; y < src_height; y++) {
        for (int x = 0; x < src_width; x++) {
            for (int lab = 0; lab < 3; lab++) {
                src_buf = src_lab.at<cv::Vec3d>(y,x)[lab];
                lab_src_sum[lab] += src_buf;
                lab_square_src_sum[lab] += src_buf * src_buf;
            }
        }
    }
    
    //calculate lab_ref_sum, lab_square_ref_sum
    double lab_ref_sum[3] = {0.0, 0.0, 0.0};
    double lab_square_ref_sum[3] = {0.0, 0.0, 0.0};
    double ref_buf = 0.0;
    for (int y = 0; y < ref_height; y++) {
        for (int x = 0; x < ref_width; x++) {
            for (int lab = 0; lab < 3; lab++) {
                ref_buf = ref_lab.at<cv::Vec3d>(y,x)[lab];
                lab_ref_sum[lab] += ref_buf;
                lab_square_ref_sum[lab] += ref_buf * ref_buf;
            }
        }
    }
    
    //calculate lab_src_mean, lab_square_src_mean, lab_ref_mean, lab_square_ref_mean
    double lab_src_mean[3] = {0.0, 0.0, 0.0};
    double lab_ref_mean[3] = {0.0, 0.0, 0.0};
    double lab_square_src_mean[3] = {0.0, 0.0, 0.0};
    double lab_square_ref_mean[3] = {0.0, 0.0, 0.0};
    for (int lab = 0; lab < 3; lab++){
        lab_src_mean[lab] = lab_src_sum[lab] / (src_height * src_width);
        lab_ref_mean[lab] = lab_ref_sum[lab] / (ref_height * ref_width);
        lab_square_src_mean[lab] = lab_square_src_sum[lab] / (src_height * src_width);
        lab_square_ref_mean[lab] = lab_square_ref_sum[lab] / (ref_height * ref_width);
    }
    
    //calculate lab_sigma_src, lab_sigma_ref
    double lab_sigma_src[3] = {0.0, 0.0, 0.0};
    double lab_sigma_ref[3] = {0.0, 0.0, 0.0};
    for (int lab = 0; lab < 3; lab++){
        lab_sigma_src[lab] = sqrt(lab_square_src_mean[lab] - lab_src_mean[lab] * lab_src_mean[lab]);
        lab_sigma_ref[lab] = sqrt(lab_square_ref_mean[lab] - lab_ref_mean[lab] * lab_ref_mean[lab]);
    }
    
    //calculate dst_lab
    cv::Mat dst_lab = cv::Mat::zeros(src_bgr.size(), CV_64FC3);
    for (int y = 0; y < src_height; y++) {
        for (int x = 0; x < src_width; x++) {
            for (int lab = 0; lab < 3; lab++) {
                dst_lab.at<cv::Vec3d>(y,x)[lab] = lab_sigma_ref[lab] / lab_sigma_src[lab] * (src_lab.at<cv::Vec3d>(y,x)[lab] - lab_src_mean[lab]) + lab_ref_mean[lab];
            }
        }
    }
    //cconvert dst_img lab to bgr
    cv::Mat dst_bgr = cv::Mat::zeros(src_bgr.size(), CV_64FC3);
    for (int y = 0; y < dst_bgr.rows; y++) {
        for (int x = 0; x < dst_bgr.cols; x++) {
            dst_bgr.at<cv::Vec3d>(y,x) = lab2BGR(dst_lab.at<cv::Vec3d>(y,x));
        }
    }
    
    std::string title;
    std::cout << "保存する画像の名前を入力してください" << std::endl;
    std::cin >> title;
    
    //convert double -> uchar
    dst_bgr.convertTo(dst_bgr, CV_8UC3, 255.0);
    cv::imshow("src_bgr", src_bgr);
    cv::imshow("ref_bgr", ref_bgr);
    cv::imshow("dst_bgr", dst_bgr);
    cv::imwrite(title + ".jpg",dst_bgr);
    cv::waitKey(0);
}

//
// Created by Yaman SirajAldeen on 7/11/23.
//
#include <opencv2/opencv.hpp>
#include <iostream>
#include "BitBuffer.h"

int main() {
    qr_io::BitBuffer t{};
    using namespace std;

    t.appendBits(40, 6);

    cout << t;

//    t.appendBits(40, 32);

    cv::Mat image(100, 100, CV_8UC3, cv::Scalar(0, 255, 0));

//    std::cout << "PASSED" << std::endl;
    cv::imshow("Test Image", image);
    cv::waitKey(0);

    return 0;
}

//
// Created by Yaman SirajAldeen on 7/11/23.
//
#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    cv::Mat image(100, 100, CV_8UC3, cv::Scalar(0, 255, 0));

//    std::cout << "PASSED" << std::endl;
    cv::imshow("Test Image", image);
    cv::waitKey(0);

    return 0;


}

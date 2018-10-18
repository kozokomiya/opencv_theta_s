//
//  main.cpp
//  EquirectangularConversion
//
//  Created by Kozo Komiya on 2018/10/18.
//  Copyright © 2018 Kozo Komiya. All rights reserved.
//

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "ThetaConversion.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    if (argc < 3) {
        cerr << "Equirectangular conversion for Theta S" << endl;
        cerr << endl;
        cerr << "Usage:" << endl;
        cerr << "$ ec_thetas <input file> <output file>" << endl;
        return -1;
    }
    string input_file  = argv[1];
    string output_file = argv[2];
    cout << "Input file  : " << input_file  << endl;
    cout << "Output file : " << output_file << endl;
    
    cv::VideoCapture cap(input_file);
    if (!cap.isOpened()) {
        cerr << "Error: Input file can't open. " << input_file << endl;
        return -1;
    }
    // width
    int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    // height
    int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    // number of frames
    int count = cap.get(CV_CAP_PROP_FRAME_COUNT);
    // fps
    double fps = cap.get(CV_CAP_PROP_FPS);
    
    cout << "size = " << width << "x" << height << endl;
    cout << "frame count = " << count << endl;
    cout << "fps = " << fps << endl;
    
    cv::Size size(width, height);
    int fourcc = CV_FOURCC('m', 'p', '4', 'v');  // .mp4
    cv::VideoWriter writer(output_file, fourcc, fps, size);
    
    if (!writer.isOpened()) {
        cerr << "Error: output file can't open. " << output_file << endl;
        return -1;
    }
    
    cv::Mat mat;
    ThetaConversion theta(width, height);
    for(int i = 0; ; i++) {
        cap >> mat;
        if (mat.empty()) break;
        theta.doConversion(mat);
//        cv::imshow("image", frame);
//        if (cv::waitKey(1) >= 0) break;
        writer << mat;
        if (i % 30 == 0) cout << '.' << flush;
    }
    cout << endl;
    return 0;
}

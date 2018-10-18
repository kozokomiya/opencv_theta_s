//
//  ThetaConversion.cpp
//  EquirectangularConversion
//
//  Created by Kozo Komiya on 2018/10/18.
//  Copyright © 2018 Kozo Komiya. All rights reserved.
//

#include "ThetaConversion.hpp"

using namespace cv;
using namespace std;

// Constructor
ThetaConversion::ThetaConversion(int _w, int _h)
{
    cols = _w;
    rows = _h;
    map_x = new Mat(Size(cols, rows), CV_32FC1);
    map_y = new Mat(Size(cols, rows), CV_32FC1);
    makeMap();
}

// Make mapping table for equirectangular conversion
void ThetaConversion::makeMap()
{
    float dst_y = float(cols)/2;
    float src_cx = float(cols)/4;
    float src_cy = float(cols)/4;
    float src_r = 0.884 * float(cols) / 4;
    float src_rx = src_r * 1.00;       // 7.11
    float src_ry = src_r * 1.00;       // 7.11
    float src_cx2 = cols - src_cx;
    
    // make mapping table
    for(int y = 0; y < dst_y; y++) {
        for(int x = 0; x < cols; x++) {
            float ph1 = M_PI * x / dst_y;
            float th1 = M_PI * y / dst_y;
            
            float x1 = sin(th1) * cos(ph1);
            float y1 = sin(th1) * sin(ph1);
            float z = cos(th1);
            
            float ph2 = acos(-x1);
            float th2 = (y1 >= 0 ? 1 : -1) * acos(-z / sqrt(y1 * y1 + z * z));
            
            float r0;
            if (ph2 < M_PI / 2) {
                r0 = ph2 / (M_PI / 2);              // Equidistant projection
                // r0 = tan(ph2 / 2);               // Stereographic projection
                map_x->at<float>(y, x) = src_rx * r0 * cos(th2) + src_cx;
                map_y->at<float>(y, x) = src_ry * r0 * sin(th2) + src_cy;
            } else {
                r0 = (M_PI - ph2) / (M_PI / 2);     // Equidistant projection
                // r0 = tan((M_PI - ph2) / 2);      // Stereographic projection
                map_x->at<float>(y, x) = src_rx * r0 * cos(M_PI - th2) + src_cx2;
                map_y->at<float>(y, x) = src_ry * r0 * sin(M_PI - th2) + src_cy;
            }
        }
    }
}

void ThetaConversion::doConversion(cv::Mat &mat)
{
    equirectangularConversion(mat);
    overlaySizeInfo(mat);
}

void ThetaConversion::overlaySizeInfo(cv::Mat &mat)
{
    string s = "Size: " + to_string(cols) + "x" + to_string(rows);
    putText(mat, s, Point(20, 80), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(20,230,20), 2, CV_AA);
}

void ThetaConversion::equirectangularConversion(cv::Mat &mat)
{
    Mat buf = Mat(mat.size(), mat.type());
    remap(mat, buf, *map_x, *map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0,0));
    buf.copyTo(mat);
}

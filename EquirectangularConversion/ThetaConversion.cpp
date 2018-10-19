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

void ThetaConversion::doConversion(Mat &mat)
{
    equirectangularConversion(mat);
    antiRotate(mat);
    overlaySizeInfo(mat);
}

void ThetaConversion::overlaySizeInfo(Mat &mat)
{
    string s = "Size: " + to_string(cols) + "x" + to_string(rows);
    putText(mat, s, Point(20, 80), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(20,230,20), 2, CV_AA);
}

void ThetaConversion::equirectangularConversion(Mat &mat)
{
    Mat buf = Mat(mat.size(), mat.type());
    remap(mat, buf, *map_x, *map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0,0));
    buf.copyTo(mat);
}

void ThetaConversion::antiRotate(Mat &mat)
{
    shift += diffRotate(mat);
    Mat buf;
    
    if (shift >= cols) shift -= cols;
    if (shift <  0   ) shift += cols;
    if (shift != 0) {
        Mat buf;
        Rect r1(Point(0    , 0), Size(shift       , rows));
        Rect r2(Point(shift, 0), Size(cols - shift, rows));
        hconcat(mat(r2), mat(r1), buf);
        buf.copyTo(mat);
    }
}

int ThetaConversion::diffRotate(Mat& mat)
{
    const int y1 = rows / 2;
    Rect r1(0, y1, cols, 1);
    Mat l1(mat, r1);
    Mat m1;
    cvtColor(l1, m1, CV_RGB2GRAY);
    int ret = 0;

    if (prev.cols == cols) {
        int w0 = cols / 12;
        Rect rs0(0, 0, w0, 1);
        Mat t1;
        hconcat(prev, prev(rs0), t1);
        Rect rs1(cols - w0, 0, w0, 1);
        Mat b1;
        hconcat(prev(rs1), t1, b1);
        Mat result1;
        matchTemplate(b1, m1, result1, CV_TM_CCOEFF);
        Point p1;
        double v1;
        minMaxLoc(result1, NULL, &v1, NULL, &p1);
        ret = w0 - p1.x;
    }

    m1.copyTo(prev);
    return ret;
}

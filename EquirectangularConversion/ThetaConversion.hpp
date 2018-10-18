//
//  ThetaConversion.hpp
//  EquirectangularConversion
//
//  Created by Kozo Komiya on 2018/10/18.
//  Copyright © 2018 Kozo Komiya. All rights reserved.
//

#ifndef ThetaConversion_hpp
#define ThetaConversion_hpp

#include "ThetaConversion.hpp"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

class ThetaConversion {
public:
    ThetaConversion(int _w, int _h);
    void makeMap();
    void doConversion(cv::Mat &mat);
    void overlaySizeInfo(cv::Mat &mat);
    void equirectangularConversion(cv::Mat &mat);
private:
    int cols;
    int rows;
    cv::Mat* map_x;
    cv::Mat* map_y;
};


#endif /* ThetaConversion_hpp */

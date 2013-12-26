//
//  OpenCvUtils.h
//  ImgMapper
//
//  Created by nicolas lattuada on 26/12/13.
//  Copyright (c) 2013 nicolas lattuada. All rights reserved.
//

#ifndef __ImgMapper__OpenCvUtils__
#define __ImgMapper__OpenCvUtils__

#define CV_MATCH_METHOD 3

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include "DataModel.h"

class OpenCvUtils
{
    
private:
    int i;
    
public:
    OpenCvUtils(){};
    
    DataModel::AreaElementResult *matchTemplate(cv::Mat *imgMat, DataModel::AreaElement *elt);
    DataModel::AreaElementResult *diffPixels(cv::Mat *imgMat, DataModel::AreaElement *elt);
    DataModel::AreaElementResult *GetUTF8Text(cv::Mat *imgMat, DataModel::AreaElement *elt);
    void debugImage(cv::Mat img);
    bool matIsEqual(const cv::Mat mat1, const cv::Mat mat2);
};


#endif /* defined(__ImgMapper__OpenCvUtils__) */

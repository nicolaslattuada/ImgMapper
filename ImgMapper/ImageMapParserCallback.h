//
//  ImageMapParserCallback.h
//  ImgMapper
//
//  Created by nicolas lattuada on 26/12/13.
//  Copyright (c) 2013 nicolas lattuada. All rights reserved.
//

#ifndef __ImgMapper__ImageMapParserCallback__
#define __ImgMapper__ImageMapParserCallback__

#include <iostream>
#include <list>

#include <opencv2/imgproc/imgproc.hpp>

#include "DataModel.h"
#include "OpenCvUtils.h"

class ImageMapParserCallback
{

private:
    cv::Mat *imgMat;
    OpenCvUtils *cvUtils;
    std::list<DataModel::AreaElementResult*> result;
    
public:
        
    void StartElement( const char *name, const char** atts );
    
    void EndElement( const char* name );
    
    void setImgMat(cv::Mat *m) { this->imgMat = m; };
    cv::Mat *getImgMat() { return this->imgMat; };

    void processElement(DataModel::AreaElement *elt);


    void setCvUtils(OpenCvUtils *cvUtils) { this->cvUtils = cvUtils; };
    OpenCvUtils *getCvUtils() { return this->cvUtils; };

    std::list<DataModel::AreaElementResult*> getResult() { return result; };
    
};

#endif /* defined(__ImgMapper__ImageMapParserCallback__) */

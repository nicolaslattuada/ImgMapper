//
//  ImageMapParser.h
//  ImgMapper
//
//  Created by nicolas lattuada on 26/12/13.
//  Copyright (c) 2013 nicolas lattuada. All rights reserved.
//

#ifndef __ImgMapper__ImageMapParser__
#define __ImgMapper__ImageMapParser__

#include <iostream>
#include <expat.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "ImageMapParserCallback.h"

#define BUFFER_SIZE 100000


class ImageMapParser
{
    
private:
    std::string imageFileName;
    cv::Mat imgMat;
    std::string xmlFileName;
    ImageMapParserCallback *parserCallback;
    
public:    
    //! Constructor
    ImageMapParser(){}
    //! Destructor
    virtual ~ImageMapParser(){}
    
    void setImgMat(cv::Mat m) { imgMat = m; }
    cv::Mat getImgMat() { return imgMat; }
    
    void setImageFileName(std::string f) { imageFileName = f; }
    std::string getImageFileName() { return imageFileName; }
    
    void setXmlFileName(std::string f) { xmlFileName = f; }
    std::string getXmlFileName() { return xmlFileName; }
    
    void setParserCallback(ImageMapParserCallback *cb) { parserCallback = cb; }
    ImageMapParserCallback *getParserCallback() { return parserCallback; }
    
    int Parse();
};


#endif /* defined(__ImgMapper__ImageMapParser__) */

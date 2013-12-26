//
//  OpenCvUtils.cpp
//  ImgMapper
//
//  Created by nicolas lattuada on 26/12/13.
//  Copyright (c) 2013 nicolas lattuada. All rights reserved.
//

#include "OpenCvUtils.h"

DataModel::AreaElementResult *OpenCvUtils::matchTemplate(cv::Mat *imgMat, DataModel::AreaElement *elt)
{
    cv::Mat templ = cv::imread(elt->href, 1);
    cv::Mat imgMatRef = *imgMat;
    cv::Mat imgRoi = imgMatRef(cv::Rect(elt->coords[0],
                                        elt->coords[1],
                                        (elt->coords[2] - elt->coords[0]), // Width.
                                        (elt->coords[3] - elt->coords[1]) // Height.
                                        ));
    cv::Mat result;
    
    /// Create the result matrix
    int resultCols =  imgMat->cols - templ.cols + 1;
    int resultRows = imgMat->rows - templ.rows + 1;
    result.create(resultCols, resultRows, CV_32FC1 );
    
    cv::matchTemplate(imgRoi, templ, result, CV_MATCH_METHOD);
    cv::threshold(result, result, 0.3, 1., CV_THRESH_TOZERO);
    
    /// Localizing the best match with minMaxLoc
    double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc; cv::Point matchLoc;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
    matchLoc = maxLoc; // 0,1=> min, 2,3,4,5=>max
    rectangle(imgRoi,
              matchLoc,
              cv::Point(matchLoc.x + templ.cols , matchLoc.y + templ.rows),
              cv::Scalar::all(0), 2, 8, 0);
    
    int resultSize = *(result.size.p);
    
    DataModel::AreaElementResult *areaResult = new DataModel::AreaElementResult();
    areaResult->id = elt->id;
    areaResult->x = matchLoc.x;
    areaResult->y = matchLoc.y;
    
    if (resultSize > elt->value) {
        areaResult->value = "success";
    } else {
        areaResult->value = "fail";
    }
    
    return areaResult;
}

DataModel::AreaElementResult *OpenCvUtils::diffPixels(cv::Mat *imgMat, DataModel::AreaElement *elt)
{
    cv::Mat imgMatRef = *imgMat;
    cv::Mat templ = cv::imread(elt->href, 1);
    int roiWidth = elt->coords[2] - elt->coords[0];
    int roiHeight = elt->coords[3] - elt->coords[1];
    int xRange = roiWidth - templ.cols;
    int yRange = roiHeight - templ.rows;
    cv::Mat imgRoi;
    DataModel::AreaElementResult *areaResult = new DataModel::AreaElementResult();
    areaResult->id = elt->id;
    
    for (int x=0; x<xRange; x++) {
        for (int y = 0; y<yRange; y++) {
            imgRoi = imgMatRef(cv::Rect(elt->coords[0] + x, elt->coords[1] + y, templ.cols, templ.rows));
            if (matIsEqual(templ, imgRoi)) {
                areaResult->x = x;
                areaResult->y = y;
                areaResult->value = "success";

                return areaResult;
            }
        }
    }
    
    areaResult->value = "fail";

    return areaResult;
}

DataModel::AreaElementResult *OpenCvUtils::GetUTF8Text(cv::Mat *imgMat, DataModel::AreaElement *elt)
{
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    DataModel::AreaElementResult *areaResult = new DataModel::AreaElementResult();
    areaResult->id = elt->id;

    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init(NULL, "eng")) {
        areaResult->value = "Could not initialize tesseract.";
        return areaResult;
    }
    
    int step1 = (int) imgMat->step1();
    api->SetImage((uchar*)imgMat->data, imgMat->size().width, imgMat->size().height, imgMat->channels(), step1);
    api->SetRectangle(elt->coords[0],
                      elt->coords[1],
                      (elt->coords[2] - elt->coords[0]), // Width.
                      (elt->coords[3] - elt->coords[1]) // Height.
                      );
    std::string boxText = api->GetUTF8Text();
    // Right trim.
    boxText.erase(std::find_if(boxText.rbegin(), boxText.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), boxText.end());
    areaResult->value = boxText;
    
    return areaResult;
}

void OpenCvUtils::debugImage(cv::Mat img)
{
    const char* window = "Debug window";
    cv::namedWindow(window, CV_WINDOW_AUTOSIZE);
    cv::imshow(window, img);
    cv::waitKey(0);
}

bool OpenCvUtils::matIsEqual(const cv::Mat mat1, const cv::Mat mat2)
{
    // treat two empty mat as identical as well
    if (mat1.empty() && mat2.empty()) {
        return true;
    }
    // if dimensionality of two mat is not identical, these two mat is not identical
    if (mat1.cols != mat2.cols || mat1.rows != mat2.rows || mat1.dims != mat2.dims) {
        return false;
    }
    
    cv::Mat diff;    cv::Mat diff1color;
    cv::compare(mat1, mat2, diff, cv::CMP_NE);
    cv::cvtColor(diff, diff1color, CV_BGRA2GRAY, 1);
    
    int nz = cv::countNonZero(diff1color);
    return nz==0;
}


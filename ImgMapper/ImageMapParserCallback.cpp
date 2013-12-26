//
//  ImageMapParserCallback.cpp
//  ImgMapper
//
//  Created by nicolas lattuada on 26/12/13.
//  Copyright (c) 2013 nicolas lattuada. All rights reserved.
//

#include "ImageMapParserCallback.h"


DataModel::AreaElement *createAreaElement(const char **attribute)
{    
    DataModel::AreaElement *elt = new DataModel::AreaElement();
    int i;
    int eltsInitialized = 0;
    for (i = 0; attribute[i]; i += 2) {
        std::string attrName(attribute[i]);
        std::string attrValue(attribute[i+1]);
        if (attrName=="target") {
            elt->id = attrValue;
            eltsInitialized++;
        }
        if (attrName=="coords") {
            std::stringstream ss(attrValue);
            int i;
            while (ss >> i) {
                elt->coords.push_back(i);
                if (ss.peek() == ',') {
                    ss.ignore();
                }
            }
            
            eltsInitialized++;
        }
        if (attrName=="href" || attrName=="nohref") {
            elt->href = attrValue;
            eltsInitialized++;
        }
        if (attrName=="alt") {
            elt->alt = attrValue;
            eltsInitialized++;
        }
        if (attrName=="value") {
            // Optionnal.
            elt->value = atoi(attrValue.c_str());
        }
    }
    
    if (eltsInitialized>=4) {
        elt->valid = true;
    }
    
    return elt;
}


void ImageMapParserCallback::processElement(DataModel::AreaElement *elt)
{
    DataModel::AreaElementResult *areaResult = NULL;
    if (elt->alt=="matchTemplate") {
        areaResult = this->getCvUtils()->matchTemplate(this->getImgMat(), elt);
    }
    
    if (elt->alt=="diffPixels") {
        areaResult = this->getCvUtils()->diffPixels(this->getImgMat(), elt);
    }
    
    if (elt->alt=="GetUTF8Text") {
        areaResult = this->getCvUtils()->GetUTF8Text(this->getImgMat(), elt);
    }
    
    this->result.push_back(areaResult);
}

void ImageMapParserCallback::StartElement(const char *name, const char** attributes)
{
    std::string tagName(name);
    if (tagName=="area") {
        DataModel::AreaElement *curAreaElt = createAreaElement(attributes);
        if (curAreaElt->valid) {
            processElement(curAreaElt);
        } else {
            std::cout << "\"" << curAreaElt->id  << "\",\"not valid\"" << std::endl;
        }
    }
}

void ImageMapParserCallback::EndElement(const char *name)
{
    
}
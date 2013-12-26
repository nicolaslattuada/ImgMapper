//
//  DataModel.h
//  ImgMapper
//
//  Created by nicolas lattuada on 26/12/13.
//  Copyright (c) 2013 nicolas lattuada. All rights reserved.
//

#ifndef __ImgMapper__DataModel__
#define __ImgMapper__DataModel__

#define CV_MATCH_MIN_RESULT_SIZE 200

#include <iostream>
#include <vector>

class DataModel
{
public:
    struct AreaElement {
        std::string id = "";
        std::vector<double> coords;
        std::string href = "";
        std::string alt = "";
        int value = CV_MATCH_MIN_RESULT_SIZE;
        bool valid = false;
    };
    
    struct AreaElementResult {
        std::string id = "";
        double x;
        double y;
        std::string value = "";
    };
    
};
#endif /* defined(__ImgMapper__DataModel__) */

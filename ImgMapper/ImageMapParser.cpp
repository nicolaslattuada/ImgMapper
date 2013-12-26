//
//  ImageMapParser.cpp
//  ImgMapper
//
//  Created by nicolas lattuada on 26/12/13.
//  Copyright (c) 2013 nicolas lattuada. All rights reserved.
//

#include "ImageMapParser.h"

ImageMapParserCallback *callback;

/* first when start element is encountered */
void startElement(void *data, const char *element, const char **attribute)
{
    callback->StartElement(element, attribute);
}

/* decrement the current level of the tree */
void endElement(void *data, const char *element)
{
    callback->EndElement(element);
}

int ImageMapParser::Parse()
{
    callback = this->getParserCallback();
    FILE *fp;
    char buff[BUFFER_SIZE];
    fp = fopen(this->getXmlFileName().c_str(), "rb");
    if (fp == NULL) {
        std::cout << "Failed to open file: " << this->getXmlFileName() << std::endl;
        return 1;
    }
    
    // Skip 1st line where image tag is.
    fgets(buff, BUFFER_SIZE, fp);
    
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetElementHandler(parser, startElement, endElement);
    
    fread(buff, sizeof(char), BUFFER_SIZE, fp);
    
    /* parse the xml */
    if (XML_Parse(parser, buff, (int) strlen(buff), XML_TRUE) == XML_STATUS_ERROR) {
        printf("Error: %s\n", XML_ErrorString(XML_GetErrorCode(parser)));
    }
    
    fclose(fp);
    XML_ParserFree(parser);
    
    return 0;
}


#include <expat.h>
#include <string>
#include <vector>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#define BUFFER_SIZE 100000
#define CV_MATCH_METHOD 3
#define CV_MATCH_MIN_RESULT_SIZE 200

/* track the current level in the xml tree */
static int depth = 0;
std::string imageFileName;
cv::Mat imgMat;
std::string xmlFileName;

struct AreaElement {
    std::string id = "";
    std::vector<double> coords;
    std::string href = "";
    std::string alt = "";
    int value = CV_MATCH_MIN_RESULT_SIZE;
    bool valid = false;
};

AreaElement *createAreaElement(const char **attribute) {
    AreaElement *elt = new AreaElement;
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

void debugImage(cv::Mat img)
{
    const char* window = "Debug window";
    cv::namedWindow(window, CV_WINDOW_AUTOSIZE);
    cv::imshow(window, img);
    cv::waitKey(0);
}

bool matIsEqual(const cv::Mat mat1, const cv::Mat mat2){
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


void processElement(AreaElement *elt)
{
    if (elt->alt=="matchTemplate") {
        cv::Mat templ = cv::imread(elt->href, 1);
        cv::Mat imgRoi = imgMat(cv::Rect(elt->coords[0],
                                         elt->coords[1],
                                         (elt->coords[2] - elt->coords[0]), // Width.
                                         (elt->coords[3] - elt->coords[1]) // Height.
                                ));
        cv::Mat result;

        /// Create the result matrix
        int resultCols =  imgMat.cols - templ.cols + 1;
        int resultRows = imgMat.rows - templ.rows + 1;
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

        if (resultSize > elt->value) {
            std::cout << "\"" << elt->id  << "\",\"success\"," << resultSize << std::endl;
        } else {
            std::cout << "\"" << elt->id  << ",\"fail\"," << resultSize << std::endl;
        }
    }
    
    if (elt->alt=="diffPixels") {
        cv::Mat templ = cv::imread(elt->href, 1);
        int roiWidth = elt->coords[2] - elt->coords[0];
        int roiHeight = elt->coords[3] - elt->coords[1];
        int xRange = roiWidth - templ.cols;
        int yRange = roiHeight - templ.rows;
        cv::Mat imgRoi;
        for (int x=0; x<xRange; x++) {
            for (int y = 0; y<yRange; y++) {
                imgRoi = imgMat(cv::Rect(elt->coords[0] + x, elt->coords[1] + y, templ.cols, templ.rows));
                if (matIsEqual(templ, imgRoi)) {
                    std::cout << "\"" << elt->id  << "\",\"success\"," << x << "," << y << std::endl;
                    return;
                }
            }
        }

        std::cout << "\"" << elt->id  << ",\"fail\"" << std::endl;

    }
    
    if (elt->alt=="GetUTF8Text") {
        tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
        // Initialize tesseract-ocr with English, without specifying tessdata path
        if (api->Init(NULL, "eng")) {
            std::cout << "Could not initialize tesseract." << std::endl;
            return;
        }
        int step1 = (int) imgMat.step1();
        api->SetImage((uchar*)imgMat.data, imgMat.size().width, imgMat.size().height, imgMat.channels(), step1);
        api->SetRectangle(elt->coords[0],
                          elt->coords[1],
                          (elt->coords[2] - elt->coords[0]), // Width.
                          (elt->coords[3] - elt->coords[1]) // Height.
                          );
        std::string boxText = api->GetUTF8Text();
        // Right trim.
        boxText.erase(std::find_if(boxText.rbegin(), boxText.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), boxText.end());
        std::cout << "\"" << elt->id  << "\",\"" << boxText << "\"" << std::endl;
    }
}

/* first when start element is encountered */
void startElement(void *data, const char *element, const char **attribute)
{
    std::string tagName(element);
    if (tagName=="area") {
        AreaElement *curAreaElt = createAreaElement(attribute);
        if (curAreaElt->valid) {
            processElement(curAreaElt);
        } else {
            std::cout << "\"" << curAreaElt->id  << "\",\"not valid\"" << std::endl;
        }
    }

    depth++;
}

/* decrement the current level of the tree */
void endElement(void *data, const char *el)
{
    depth--;
}

int parseXml(std::string xmlFileName)
{
    FILE *fp;
    char buff[BUFFER_SIZE];
    fp = fopen(xmlFileName.c_str(), "rb");
    if (fp == NULL) {
        std::cout << "Failed to open file: " << xmlFileName << std::endl;
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


void readme()
{
    std::cout << "following params are required: " << std::endl;
    std::cout << "-file <file>  -- Path to image file to analyze" << std::endl;
    std::cout << "-map <file>  -- Path to html image map file that contains instructions" << std::endl;
}

int main(int argc, char **argv)
{
    if(argc < 5) {
        readme();
        return 1;
    }
    
    int i=1;
    while(i<argc) {
        if(strcmp(argv[i],"-file")==0) {
            imageFileName = argv[++i];
        }
        if(strcmp(argv[i],"-map")==0) {
            xmlFileName = argv[++i];
        }
        
        i++;
    }
    
    imgMat = cv::imread(imageFileName, 1);
    
    int result = parseXml(xmlFileName);

    return result;
}
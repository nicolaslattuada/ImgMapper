#include "ImageMapParser.h"
#include "OpenCvUtils.h"
#include "HandEvaluator.h"

std::string imageFileName;
cv::Mat imgMat;
std::string xmlFileName;

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
    
    OpenCvUtils *cvUtils = new OpenCvUtils();
    ImageMapParserCallback *parserCallback = new ImageMapParserCallback();
    parserCallback->setImgMat(&imgMat);
    parserCallback->setCvUtils(cvUtils);
    
    ImageMapParser *parser = new ImageMapParser();
    parser->setParserCallback(parserCallback);
    parser->setXmlFileName(xmlFileName);
    parser->Parse();

    std::list<DataModel::AreaElementResult*> parsingResult = parserCallback->getResult();
    std::list<DataModel::AreaElementResult*>::iterator it;
    it = parsingResult.begin();
    while (it != parsingResult.end()) {
        std::cout << (**it).id << ":" << (**it).value << " " << (**it).x << " " << (**it).y << std::endl;
        it++;
    }
    
    std::string board = "AdAsKs";
    std::string myCards = "QsJs";
    HandEvaluator *handEvaluator = new HandEvaluator();
    //StdDeck_CardMask cardMask = handEvaluator->textToPokerEval(board.c_str());
    handEvaluator->evalHand(myCards.c_str(), board.c_str());
    
    return 0;
}
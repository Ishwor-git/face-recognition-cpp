#ifndef SMILE_DETECTOR
#define SMILE_DETECTOR

#include "detector.hpp"

class SmileDetector : public Detector
{
public:
    SmileDetector(const std::string &classifierPath) : Detector(classifierPath){};

    void detect(const Mat &img) override
    {
        //  an empty implementation
    }

    // This method is specific to SmileDetector and does not override the base class's detect method.
    void detect(const Mat &img, const Rect &faceRegion)
    {
        Mat ROI = img(faceRegion);
        vector<Rect> smiles;
        classifier.detectMultiScale(ROI, smiles, 1.7, 20, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
        for (const auto &smile : smiles)
        {
            Point center(faceRegion.x + smile.x + smile.width / 2, faceRegion.y + smile.y + smile.height / 2);
            ellipse(img, center, Size(smile.width / 2, smile.height / 2), 0, 0, 360, Scalar(255, 0, 0), 4);
        }
    }
};

#endif // SMILE_DETECTOR
#ifndef EYES_DETECTOR
#define EYES_DETECTOR

#include "detector.hpp"
#include <vector>

class EyesDetector : public Detector
{
public:
    EyesDetector(const std::string &classifierPath) : Detector(classifierPath){};

    void detect(const Mat &img) override
    {
        //  an empty implementation
    }

    // This method is specific to EyesDetector and does not override the base class's detect method.
    void detect(const Mat &img, const Rect &faceRegion)
    {
        Mat ROI = img(faceRegion);
        vector<Rect> eyes;
        classifier.detectMultiScale(ROI, eyes, 1.20, 5, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
        for (const auto &e : eyes)
        {
            Rect eyeRect(faceRegion.x + e.x, faceRegion.y + e.y, e.width, e.height);
            rectangle(img, eyeRect.tl(), eyeRect.br(), Scalar(0, 255, 0), 2);
        }
    }
};

#endif // EYES_DETECTOR
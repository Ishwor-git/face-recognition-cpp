#ifndef DETECTOR
#define DETECTOR

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

class Detector
{
protected:
    CascadeClassifier classifier;

public:
    Detector(const string &classifierPath)
    {
        if (!classifier.load(classifierPath))
        {
            std::cout << "Error loading classifier from " << classifierPath << std::endl;
        }
    }

    // Pure virtual function for detection
    virtual void detect(const Mat &img) = 0;
};

#endif // DETECTOR
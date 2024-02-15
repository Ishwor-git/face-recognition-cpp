#ifndef FACE_DETECTOR
#define FACE_DETECTOR

#include "detector.hpp"
#include <vector>

class FaceDetector : public Detector
{
private:
    std::vector<cv::Rect> lastDetectedFaces;

public:
    FaceDetector(const std::string &classifierPath) : Detector(classifierPath){};

    void detect(const Mat &img) override
    {
        vector<Rect> faces;
        classifier.detectMultiScale(img, faces, 1.3, 5);
        for (const auto &face : faces)
        {
            rectangle(img, face.tl(), face.br(), Scalar(50, 50, 255), 2);
        }
        cout << faces.size() << " face(s) found." << endl;
        lastDetectedFaces = faces;
    }

    const std::vector<cv::Rect> &getLastDetectedFaces() const
    {
        return lastDetectedFaces;
    }
};

#endif // FACE_DETECTOR
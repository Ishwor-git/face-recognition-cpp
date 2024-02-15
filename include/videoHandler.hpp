#ifndef VIDEO_HANDLER
#define VIDEO_HANDLER

#include <iostream>
#include <opencv2/opencv.hpp>

class VideoHandler
{
private:
    static VideoHandler *instance;
    cv::VideoCapture video;
    std::string windowName;
    bool captureImage;

    static void onMouse(int event, int x, int y, int flags, void *userdata)
    {
        if (event == cv::EVENT_LBUTTONDOWN)
        {
            instance->captureImage = true;
        }
    }

    // Image Capturing and Saving
    void captureAndSaveImage(const cv::Mat &frame)
    {
        // Generate timestamp for filename
        std::time_t now = std::time(nullptr);
        char timestamp[20];
        std::strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", std::localtime(&now));
        std::string filename = "captured_" + std::string(timestamp) + ".jpg";

        // Save frame to file
        cv::imwrite(filename, frame);
        std::cout << "Image saved as " << filename << std::endl;
    }

public:
    // Include detector instances
    FaceDetector faceDetector;
    EyesDetector eyesDetector;
    SmileDetector smileDetector;
    EmotionDetector emotionDetector;

    // Video Handler Constructor
    VideoHandler(const std::string &windowName, const std::string &faceCascadePath,
                 const std::string &eyesCascadePath, const std::string &smileCascadePath)
        : windowName(windowName), captureImage(false),
          faceDetector(faceCascadePath),
          eyesDetector(eyesCascadePath),
          smileDetector(smileCascadePath)
    {
        instance = this;
        cv::namedWindow(windowName, cv::WINDOW_NORMAL);
        cv::setMouseCallback(windowName, onMouse, nullptr);
    }

    // Video Handler Destructor
    ~VideoHandler()
    {
        cv::destroyAllWindows();
    }

    // function to initialize camera
    bool initializeCamera(int cameraIndex)
    {
        video.open(cameraIndex);
        if (!video.isOpened())
        {
            std::cerr << "Error: Couldn't open the camera." << std::endl;
            return false;
        }
        return true;
    }

    // function to capture the video (Video Loop Frame)
    void captureLoop()
    {
        cv::Mat frame;
        while (true)
        {
            video >> frame;
            if (frame.empty())
            {
                std::cerr << "Error: Couldn't read frame from the camera." << std::endl;
                break;
            }

            if (captureImage)
            {
                captureAndSaveImage(frame);
                captureImage = false; // Reset flag

                try
                {
                    std::string emotionResponse = emotionDetector.detect(frame); // Detect emotion.
                    std::cout << "Emotion Response: " << emotionResponse << std::endl;

                    // Assuming detect() returns a JSON string response, parse it.
                    json jsonResponse = json::parse(emotionResponse);

                    // Extract detected emotion and score from the response.
                    std::string detectedEmotion = jsonResponse["emotion"];
                    double emotionScore = jsonResponse["score"];

                    // Optionally, annotate the frame with the detected emotion before displaying it.
                    annotateAndDisplayImage(frame, detectedEmotion, emotionScore);
                }
                catch (const std::exception &e)
                {
                    // Handle exceptions, which could be due to network errors, parsing errors, etc.
                    std::cerr << "Error detecting emotion: " << e.what() << std::endl;

                    // Fallback behavior or error handling.
                    annotateAndDisplayImage(frame, "Failed", 0.0);
                }
            }

            // Detect faces
            faceDetector.detect(frame);
            const auto &detectedFaces = faceDetector.getLastDetectedFaces();

            // Detect eyes and smiles within each detected face region
            for (const auto &face : detectedFaces)
            {
                eyesDetector.detect(frame, face);
                smileDetector.detect(frame, face);
            }

            cv::imshow(windowName, frame);
            char key = cv::waitKey(1);
            if (key == 27 || cv::getWindowProperty(windowName, cv::WND_PROP_VISIBLE) < 1)
            {
                break;
            }
        }
    }
};

#endif // VIDEO_HANDLER
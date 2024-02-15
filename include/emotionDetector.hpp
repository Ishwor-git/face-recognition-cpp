#ifndef EMOTION_DETECTOR
#define EMOTION_DETECTOR

#include <iostream>
#include <vector>
#include <curl/curl.h>
#include <string>
// Include nlohmann/json library
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using std::string;
using std::vector;

class EmotionDetector
{
public:
    EmotionDetector()
    {
        curl_global_init(CURL_GLOBAL_ALL); //  application initialization
    }

    ~EmotionDetector()
    {
        curl_global_cleanup(); //  application cleanup
    }

    std::string detect(const cv::Mat &faceImage)
    {
        CURL *curl = curl_easy_init();
        if (!curl)
        {
            std::cerr << "Curl initialization failed." << std::endl;
            return "";
        }

        std::string response_string;
        std::string encoded_image = image_to_base64(faceImage);

        std::string url = "http://localhost:3000/recognize";
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        json j;
        j["imageBuffer"] = encoded_image;
        std::string postData = j.dump();

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        return response_string;
    }

private:
    static size_t write_callback(void *contents, size_t size, size_t nmemb, std::string *s)
    {
        size_t newLength = size * nmemb;
        try
        {
            s->append((char *)contents, newLength);
        }
        catch (std::bad_alloc &e)
        {
            // Handle memory problem
            return 0;
        }
        return newLength;
    }

    std::string base64_encode(unsigned char const *bytes_to_encode, unsigned int in_len)
    {
        static const char *base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

        std::string ret;
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        while (in_len--)
        {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3)
            {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; (i < 4); i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i)
        {
            for (j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; (j < i + 1); j++)
                ret += base64_chars[char_array_4[j]];

            while ((i++ < 3))
                ret += '=';
        }

        return ret;
    }

    std::string image_to_base64(const cv::Mat &img)
    {
        std::vector<uchar> buf;
        cv::imencode(".jpg", img, buf);
        auto *enc_msg = reinterpret_cast<unsigned char *>(buf.data());
        return base64_encode(enc_msg, buf.size());
    }
};

#endif // EMOTION_DETECTOR
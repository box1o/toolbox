#include "ct/base/logger/logger.hpp"
#include <cstdlib>
#include <filesystem>
#include <vector>

#include <ct/base/base.hpp>
#include <ct/vision/vision.hpp>

#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

using namespace ct;

constexpr int kFrameWidth = 1080;
constexpr int kFrameHeight = 720;

cv::Mat process_frame(const cv::Mat& frame_bgr) {
    cv::Mat resized;
    cv::resize(frame_bgr, resized, cv::Size(kFrameWidth, kFrameHeight), 0.0, 0.0, cv::INTER_AREA);
    cv::Mat gray;
    cv::cvtColor(resized, gray, cv::COLOR_BGR2GRAY);

    std::vector<cv::Point2f> corners;
    cv::goodFeaturesToTrack(gray, corners, 1000, 0.01, 10);

    for (size_t i = 0; i < corners.size(); i++) {
        cv::circle(resized, corners[i], 3, cv::Scalar(0, 255, 0), -1);
    }

    return resized;
}

int main(int /*argc*/, char* /*argv*/[]) {
    log::Configure();

    const std::filesystem::path videoPath = "/home/toor/dev/toolbox/dataset/video.mp4";

    cv::VideoCapture cap(videoPath.string());
    if (!cap.isOpened()) {
        log::Error("Error: Could not open video: {}", videoPath.string());
        return EXIT_FAILURE;
    }

    cv::Mat frame;
    while (true) {
        if (!cap.read(frame) || frame.empty()) {
            log::Info("End of video or failed to read frame.");
            break;
        }

        cv::Mat vis = process_frame(frame);
        cv::imshow("Camera Feed", vis);

        const int k = cv::waitKey(30);
        if (k == 'q' || k == 27) break; // q or ESC
    }

    return EXIT_SUCCESS;
}

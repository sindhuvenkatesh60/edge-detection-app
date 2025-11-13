#include <jni.h>
#include <opencv2/opencv.hpp>
#include <android/log.h>

#define LOG_TAG "EdgeDetection"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

using namespace cv;

extern "C" {

JNIEXPORT void JNICALL
Java_com_example_edgedetection_OpenGLRenderer_processFrame(
        JNIEnv *env,
        jobject /* this */,
        jlong matAddr) {
    
    Mat& frame = *(Mat*)matAddr;
    
    if (frame.empty()) {
        LOGI("Frame is empty!");
        return;
    }
    
    // Convert to grayscale
    Mat gray;
    cvtColor(frame, gray, COLOR_RGBA2GRAY);
    
    // Apply Canny edge detection
    Mat edges;
    Canny(gray, edges, 50, 150);
    
    // Convert back to RGBA for display
    cvtColor(edges, frame, COLOR_GRAY2RGBA);
    
    LOGI("Frame processed successfully!");
}

}
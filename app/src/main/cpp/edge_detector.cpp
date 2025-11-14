#include <jni.h>
#include <opencv2/opencv.hpp>
#include <android/log.h>

#define LOG_TAG "EdgeDetector"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace cv;

extern "C" {

JNIEXPORT jlong JNICALL
Java_com_example_edgedetection_EdgeDetector_nativeCreateObject(JNIEnv *env, jobject thiz) {
    return 1;
}

JNIEXPORT void JNICALL
Java_com_example_edgedetection_EdgeDetector_nativeDestroyObject(JNIEnv *env, jobject thiz, jlong native_obj) {
    // Cleanup if needed
}

JNIEXPORT jboolean JNICALL
Java_com_example_edgedetection_EdgeDetector_nativeDetectEdges(
        JNIEnv *env, jobject thiz, jlong native_obj,
        jlong input_mat_addr, jlong output_mat_addr,
        jint algorithm, jdouble thresh1, jdouble thresh2) {

    try {
        Mat &input_mat = *(Mat *) input_mat_addr;
        Mat &output_mat = *(Mat *) output_mat_addr;

        if (input_mat.empty()) {
            LOGE("Input matrix is empty");
            return JNI_FALSE;
        }

        Mat gray, blurred, edges;

        // Convert to grayscale if needed
        if (input_mat.channels() == 3) {
            cvtColor(input_mat, gray, COLOR_RGB2GRAY);
        } else if (input_mat.channels() == 4) {
            cvtColor(input_mat, gray, COLOR_RGBA2GRAY);
        } else {
            gray = input_mat.clone();
        }

        // Apply Gaussian blur
        GaussianBlur(gray, blurred, Size(5, 5), 1.5);

        switch (algorithm) {
            case 0: // Canny
                Canny(blurred, edges, thresh1, thresh2, 3, true);
                break;
            case 1: // Sobel
            {
                Mat grad_x, grad_y;
                Mat abs_grad_x, abs_grad_y;

                Sobel(blurred, grad_x, CV_16S, 1, 0, 3);
                Sobel(blurred, grad_y, CV_16S, 0, 1, 3);

                convertScaleAbs(grad_x, abs_grad_x);
                convertScaleAbs(grad_y, abs_grad_y);

                addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, edges);
            }
                break;
            case 2: // Laplacian
                Laplacian(blurred, edges, CV_16S, 3);
                convertScaleAbs(edges, edges);
                break;
            default:
                Canny(blurred, edges, thresh1, thresh2, 3, true);
                break;
        }

        // Convert single channel to 3 channels for display
        if (output_mat.channels() == 3) {
            cvtColor(edges, output_mat, COLOR_GRAY2RGB);
        } else if (output_mat.channels() == 4) {
            cvtColor(edges, output_mat, COLOR_GRAY2RGBA);
        } else {
            edges.copyTo(output_mat);
        }

        LOGI("Edge detection completed with algorithm: %d", algorithm);
        return JNI_TRUE;

    } catch (const cv::Exception &e) {
        LOGE("CV Exception in edge detection: %s", e.what());
        return JNI_FALSE;
    } catch (...) {
        LOGE("Unknown exception in edge detection");
        return JNI_FALSE;
    }
}

}//
// Created by Vinaya on 14/11/2025.
//

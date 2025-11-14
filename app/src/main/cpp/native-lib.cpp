#include <jni.h>
#include <string>
#include <android/bitmap.h>
#include <android/log.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

#define LOG_TAG "EdgeDetection"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace cv;

extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_example_edgedetection_ImageProcessor_processImage(
        JNIEnv *env,
        jobject thiz,
        jobject input_bitmap,
        jobject output_bitmap,
        jint algorithm_type,
        jdouble threshold1,
        jdouble threshold2) {

    AndroidBitmapInfo input_info, output_info;
    void *input_pixels, *output_pixels;

    try {
        // Get bitmap information
        if (AndroidBitmap_getInfo(env, input_bitmap, &input_info) != ANDROID_BITMAP_RESULT_SUCCESS) {
            LOGE("Failed to get input bitmap info");
            return JNI_FALSE;
        }

        if (AndroidBitmap_getInfo(env, output_bitmap, &output_info) != ANDROID_BITMAP_RESULT_SUCCESS) {
            LOGE("Failed to get output bitmap info");
            return JNI_FALSE;
        }

        // Lock bitmaps
        if (AndroidBitmap_lockPixels(env, input_bitmap, &input_pixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
            LOGE("Failed to lock input bitmap pixels");
            return JNI_FALSE;
        }

        if (AndroidBitmap_lockPixels(env, output_bitmap, &output_pixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
            AndroidBitmap_unlockPixels(env, input_bitmap);
            LOGE("Failed to lock output bitmap pixels");
            return JNI_FALSE;
        }

        // Create OpenCV matrices from bitmaps
        Mat input_mat(input_info.height, input_info.width, CV_8UC4, input_pixels);
        Mat output_mat(output_info.height, output_info.width, CV_8UC4, output_pixels);

        // Convert to grayscale
        Mat gray;
        cvtColor(input_mat, gray, COLOR_RGBA2GRAY);

        // Apply Gaussian blur to reduce noise
        Mat blurred;
        GaussianBlur(gray, blurred, Size(5, 5), 1.5);

        Mat edges;

        switch (algorithm_type) {
            case 0: // Canny
                Canny(blurred, edges, threshold1, threshold2, 3, true);
                break;

            case 1: { // Sobel
                Mat grad_x, grad_y;
                Mat abs_grad_x, abs_grad_y;

                Sobel(blurred, grad_x, CV_16S, 1, 0, 3);
                Sobel(blurred, grad_y, CV_16S, 0, 1, 3);

                convertScaleAbs(grad_x, abs_grad_x);
                convertScaleAbs(grad_y, abs_grad_y);

                addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, edges);
                break;
            }

            case 2: // Laplacian
                Laplacian(blurred, edges, CV_16S, 3);
                convertScaleAbs(edges, edges);
                break;

            default:
                Canny(blurred, edges, threshold1, threshold2, 3, true);
                break;
        }

        // Convert back to RGBA
        cvtColor(edges, output_mat, COLOR_GRAY2RGBA);

        // Unlock bitmaps
        AndroidBitmap_unlockPixels(env, input_bitmap);
        AndroidBitmap_unlockPixels(env, output_bitmap);

        LOGI("Edge detection completed successfully");
        return JNI_TRUE;

    } catch (const cv::Exception &e) {
        LOGE("OpenCV exception: %s", e.what());
        AndroidBitmap_unlockPixels(env, input_bitmap);
        AndroidBitmap_unlockPixels(env, output_bitmap);
        return JNI_FALSE;
    } catch (...) {
        LOGE("Unknown exception during edge detection");
        AndroidBitmap_unlockPixels(env, input_bitmap);
        AndroidBitmap_unlockPixels(env, output_bitmap);
        return JNI_FALSE;
    }
}

JNIEXPORT jstring JNICALL
Java_com_example_edgedetection_MainActivity_getOpenCVVersion(JNIEnv *env, jobject thiz) {
    std::string version = "OpenCV Version: ";
    version += CV_VERSION;
    return env->NewStringUTF(version.c_str());
}

}
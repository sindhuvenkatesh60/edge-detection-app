#include <jni.h>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <android/log.h>

#define LOG_TAG "ImageProcessor"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace cv;

extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_example_edgedetection_ImageProcessor_nativeProcessBitmap(
        JNIEnv *env, jobject thiz,
        jobject input_bitmap, jobject output_bitmap,
        jint algorithm, jdouble thresh1, jdouble thresh2) {

    AndroidBitmapInfo input_info, output_info;
    void *input_pixels = nullptr;
    void *output_pixels = nullptr;

    try {
        // Get bitmap info
        if (AndroidBitmap_getInfo(env, input_bitmap, &input_info) != ANDROID_BITMAP_RESULT_SUCCESS) {
            LOGE("Failed to get input bitmap info");
            return JNI_FALSE;
        }

        if (AndroidBitmap_getInfo(env, output_bitmap, &output_info) != ANDROID_BITMAP_RESULT_SUCCESS) {
            LOGE("Failed to get output bitmap info");
            return JNI_FALSE;
        }

        // Verify bitmap formats
        if (input_info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
            LOGE("Input bitmap format must be RGBA_8888");
            return JNI_FALSE;
        }

        if (output_info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
            LOGE("Output bitmap format must be RGBA_8888");
            return JNI_FALSE;
        }

        // Lock pixels
        if (AndroidBitmap_lockPixels(env, input_bitmap, &input_pixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
            LOGE("Failed to lock input bitmap pixels");
            return JNI_FALSE;
        }

        if (AndroidBitmap_lockPixels(env, output_bitmap, &output_pixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
            AndroidBitmap_unlockPixels(env, input_bitmap);
            LOGE("Failed to lock output bitmap pixels");
            return JNI_FALSE;
        }

        // Create OpenCV matrices
        Mat input_mat(input_info.height, input_info.width, CV_8UC4, input_pixels);
        Mat output_mat(output_info.height, output_info.width, CV_8UC4, output_pixels);

        // Process the image
        Mat gray, blurred, edges;

        // Convert to grayscale
        cvtColor(input_mat, gray, COLOR_RGBA2GRAY);

        // Apply Gaussian blur
        GaussianBlur(gray, blurred, Size(5, 5), 1.5);

        // Apply edge detection based on algorithm
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

        // Convert back to RGBA
        cvtColor(edges, output_mat, COLOR_GRAY2RGBA);

        // Unlock pixels
        AndroidBitmap_unlockPixels(env, input_bitmap);
        AndroidBitmap_unlockPixels(env, output_bitmap);

        LOGI("Image processing completed successfully");
        return JNI_TRUE;

    } catch (const cv::Exception &e) {
        LOGE("OpenCV exception: %s", e.what());
        if (input_pixels) AndroidBitmap_unlockPixels(env, input_bitmap);
        if (output_pixels) AndroidBitmap_unlockPixels(env, output_bitmap);
        return JNI_FALSE;
    } catch (...) {
        LOGE("Unknown exception during image processing");
        if (input_pixels) AndroidBitmap_unlockPixels(env, input_bitmap);
        if (output_pixels) AndroidBitmap_unlockPixels(env, output_bitmap);
        return JNI_FALSE;
    }
}

JNIEXPORT jboolean JNICALL
Java_com_example_edgedetection_ImageProcessor_nativeEnhanceImage(
        JNIEnv *env, jobject thiz,
        jobject input_bitmap, jobject output_bitmap) {

    AndroidBitmapInfo input_info, output_info;
    void *input_pixels = nullptr;
    void *output_pixels = nullptr;

    try {
        // Get bitmap info
        if (AndroidBitmap_getInfo(env, input_bitmap, &input_info) != ANDROID_BITMAP_RESULT_SUCCESS ||
            AndroidBitmap_getInfo(env, output_bitmap, &output_info) != ANDROID_BITMAP_RESULT_SUCCESS) {
            return JNI_FALSE;
        }

        // Lock pixels
        if (AndroidBitmap_lockPixels(env, input_bitmap, &input_pixels) != ANDROID_BITMAP_RESULT_SUCCESS ||
            AndroidBitmap_lockPixels(env, output_bitmap, &output_pixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
            if (input_pixels) AndroidBitmap_unlockPixels(env, input_bitmap);
            return JNI_FALSE;
        }

        Mat input_mat(input_info.height, input_info.width, CV_8UC4, input_pixels);
        Mat output_mat(output_info.height, output_info.width, CV_8UC4, output_pixels);

        // Simple image enhancement: increase contrast
        Mat lab;
        cvtColor(input_mat, lab, COLOR_RGBA2Lab);

        std::vector<Mat> lab_planes;
        split(lab, lab_planes);

        // Enhance the L channel (lightness)
        Ptr<CLAHE> clahe = createCLAHE();
        clahe->setClipLimit(4.0);
        clahe->apply(lab_planes[0], lab_planes[0]);

        merge(lab_planes, lab);
        cvtColor(lab, output_mat, COLOR_Lab2RGBA);

        AndroidBitmap_unlockPixels(env, input_bitmap);
        AndroidBitmap_unlockPixels(env, output_bitmap);

        return JNI_TRUE;

    } catch (...) {
        if (input_pixels) AndroidBitmap_unlockPixels(env, input_bitmap);
        if (output_pixels) AndroidBitmap_unlockPixels(env, output_bitmap);
        return JNI_FALSE;
    }
}

}//
// Created by Vinaya on 14/11/2025.
//

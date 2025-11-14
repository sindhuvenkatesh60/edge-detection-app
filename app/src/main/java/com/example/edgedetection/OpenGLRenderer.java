package com.example.edgedetection;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class OpenGLRenderer implements GLSurfaceView.Renderer {
    private static final String TAG = "OpenGLRenderer";

    // Native method declaration
    public native void processFrame(long matAddr);

    // Load native libraries - ONLY LOAD OUR LIBRARY FOR NOW
    static {
        // System.loadLibrary("opencv_java4"); // Comment out for now
        System.loadLibrary("edgedetection");
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Log.d(TAG, "✅ OpenGL surface created");
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GLES20.glViewport(0, 0, width, height);
        Log.d(TAG, "✅ OpenGL surface changed: " + width + "x" + height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);

        // Test native method
        processFrame(0L); // Pass 0 for now
        Log.d(TAG, "✅ OpenGL frame drawn with native call");
    }
}
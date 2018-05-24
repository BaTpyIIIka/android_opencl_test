package ru.opencl_test.alg1.gl;

import android.content.Context;
import android.graphics.Bitmap;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import ru.michaelilyin.alg1.R;

/**
 * Created by michael on 01.05.17.
 */

public class GLRendererSimple implements GLSurfaceView.Renderer {
    private static final String TAG = "OGLES20Renderer";

    private Context ctx;

    private GLQuad quad;

    private int screenW = 0;
    private int screenH = 0;

    private int curTestRun = 0;
    private int numTestRuns = 1;

    private float[] identMat = new float[16];
    private float[] projMat = new float[16];
    private float[] viewMat = new float[16];
    private float[] mvpMat = new float[16];

    private GLShader gaussShader;
    private GLShader dispShader;

    private Bitmap testedBitmap;

    private float pushMsSum;
    private float execMsSum;
    private float pullMsSum;

    private Listener listener;
    private GLSurfaceView view;

    public GLRendererSimple(Context ctx) {
        this.ctx = ctx;

        Matrix.setIdentityM(identMat, 0);
    }

    public void test(GLSurfaceView view, Bitmap bitmap, int tests, Listener listener) {
        Log.d(TAG, "Start test");
        this.numTestRuns = tests;
        this.listener = listener;

        pushMsSum = execMsSum = pullMsSum = 0.0f;

        this.view = view;

        this.testedBitmap = bitmap;

        view.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        view.requestRender();
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        GLES20.glDisable(GLES20.GL_DEPTH_TEST);
        GLES20.glEnable(GLES20.GL_CULL_FACE);
        GLES20.glCullFace(GLES20.GL_BACK);

        GLES20.glEnable(GLES20.GL_TEXTURE_2D);

        Log.i(TAG, "onSurfaceCreated");

        // create shaders
        gaussShader = new GLShader(ctx, R.raw.gauss_5x5_v, R.raw.gauss_5x5_f);
        dispShader = new GLShader(ctx, R.raw.disp_v, R.raw.disp_f);

        quad = new GLQuad();
        quad.setNumRenderPasses(1);
        quad.setSaveFrameBuffer(true);
        quad.bindShaders(gaussShader, dispShader);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int w, int h) {
        Log.i(TAG, "onSurfaceChanged with new size " + w + "x" + h);

        screenW = w;
        screenH = h;

        GLES20.glViewport(0, 0, w, h);

        float ratio = (float) w / h;

        // this projection matrix is applied to object coordinates
        // in the onDrawFrame() method
        Matrix.orthoM(projMat, 0, -ratio, ratio, -1, 1, -1, 1);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        if (testedBitmap == null) {
            return;
        }
        Log.d(TAG, "test run: " + curTestRun);

        // Redraw background color
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        // Set the camera position (View matrix)
        Matrix.setLookAtM(viewMat, 0, 0.0f, 0.0f, 2.5f, 0f, 0f, 0f, 0f, 1.0f, 0.0f);

        // Calculate the projection and view transformation
        Matrix.multiplyMM(mvpMat, 0, projMat, 0, viewMat, 0);

        // do preparations (only for measuring the times!)
        quad.clearTextures();
        quad.prepareTextures();

        pushMsSum += quad.setTextureFromBitmap(testedBitmap);
        quad.createGeometry();

        // Render
        float[] execAndPullMs = quad.render(mvpMat, screenW, screenH);
        execMsSum += execAndPullMs[0];
        pullMsSum += execAndPullMs[1];

        GLES20.glFinish();

        curTestRun++;

        Log.i(TAG, "Cur: " + curTestRun + " num: " + numTestRuns);
        if (curTestRun >= numTestRuns) {
            testsFinished();
        }
    }

    private void testsFinished() {
        float pushMsAvg = pushMsSum / (float)numTestRuns;
        float execMsAvg = execMsSum / (float)numTestRuns;
        float pullMsAvg = pullMsSum / (float)numTestRuns;

//        benchmarkRes += selectedKernelType + ","
//                + bitmapNames.get(curRunBmIndex) + ","
//                + pushMsAvg + "," + execMsAvg + "," + pullMsAvg + "\n";

        curTestRun = 0;

//        if (curRunBmIndex >= bitmaps.size()) {	// stop testing
//            Log.i(TAG, "All tests finished");
//            Log.i(TAG, benchmarkRes);
//
//            benchmarkRes = "";
//
            view.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
//        }
        listener.onFinish(pushMsAvg, execMsAvg, pullMsAvg);
    }

    public interface Listener {
        void onFinish(float pushMsAvg, float execMsAvg, float pullMsAvg);
    }
}

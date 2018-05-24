package ru.opencl_test.alg1.gl;

/**
 * Created by michael on 01.05.17.
 */

public class ImgprocJNI {
    public final static native void initGL();
    public final static native void glviewCreate();
    public final static native void glviewResize(int jarg1, int jarg2);
    public final static native void glviewDraw();
    public final static native void setInputImage(long jarg1);
    public final static native void getOutputImage(long jarg1);
    public final static native void cleanup();
    public final static native float getImagePushMs();
    public final static native float getRenderMs();
    public final static native float getImagePullMs();
}
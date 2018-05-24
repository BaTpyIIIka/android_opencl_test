#ifndef ALG_CL_TEST_H
#define ALG_CL_TEST_H

#include <jni.h>

#include "android_tools.h"

extern "C" {

jint 	Java_ru_michaelilyin_alg1_CLUtils_initCL(JNIEnv *env, jobject obj);

jint 	Java_ru_michaelilyin_alg1_CLUtils_createProg(JNIEnv *env, jobject obj,
                                                          jstring progName, jobject progSrcBuf);

jint	Java_ru_michaelilyin_alg1_CLUtils_createKernel(JNIEnv *env, jobject obj);

jint 	Java_ru_michaelilyin_alg1_CLUtils_createCmdQueue(JNIEnv *env, jobject obj);

jfloat 	Java_ru_michaelilyin_alg1_CLUtils_setKernelArgs(JNIEnv *env, jobject obj,
                                                               jint width, jint height,
                                                               jobject inputImgBuf);

jfloat 	Java_ru_michaelilyin_alg1_CLUtils_executeKernel(JNIEnv *env, jobject obj);

jfloat 	Java_ru_michaelilyin_alg1_CLUtils_getResultImg(JNIEnv *env, jobject obj,
                                                              jobject outputImgBuf);

void 	Java_ru_michaelilyin_alg1_CLUtils_dealloc(JNIEnv *env, jobject obj);


void	Java_ru_michaelilyin_alg1_CLUtils_printInfo(JNIEnv *env, jobject obj);

void Java_ru_michaelilyin_alg1_CLUtils_begin(JNIEnv *env, jobject obj);
jfloat Java_ru_michaelilyin_alg1_CLUtils_end(JNIEnv *env, jobject obj);









void Java_ru_michaelilyin_alg1_gl_ImgprocJNI_initGL(JNIEnv *env, jobject obj);

void Java_ru_michaelilyin_alg1_gl_ImgprocJNI_glviewCreate(JNIEnv *env, jobject obj);

void Java_ru_michaelilyin_alg1_gl_ImgprocJNI_glviewResize(JNIEnv *env, jobject obj, jint w, jint h);

void Java_ru_michaelilyin_alg1_gl_ImgprocJNI_glviewDraw(JNIEnv *env, jobject obj);

void Java_ru_michaelilyin_alg1_gl_ImgprocJNI_setInputImage(JNIEnv *env, jobject obj, jlong inputImgAddr);

void Java_ru_michaelilyin_alg1_gl_ImgprocJNI_getOutputImage(JNIEnv *env, jobject obj, jlong outputImgAddr);

void Java_ru_michaelilyin_alg1_gl_ImgprocJNI_cleanup(JNIEnv *env, jobject obj);

jfloat Java_ru_michaelilyin_alg1_gl_ImgprocJNI_getImagePushMs(JNIEnv *env, jobject obj);
jfloat Java_ru_michaelilyin_alg1_gl_ImgprocJNI_getRenderMs(JNIEnv *env, jobject obj);
jfloat Java_ru_michaelilyin_alg1_gl_ImgprocJNI_getImagePullMs(JNIEnv *env, jobject obj);

}

#endif

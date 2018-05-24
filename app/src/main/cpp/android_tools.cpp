//
// Created by michael on 01.05.17.
//

//#include <opencv2/core/core.hpp>
#include <GLES2/gl2.h>
#include "android_tools.h"


unsigned char* imgDataPtrFromCvMatPtrAddr(long long cvMatPtrAddr, int *w, int *h, int *chan) {
//    cv::Mat *mat = ((cv::Mat *)cvMatPtrAddr);
//
//    if (w) *w = mat->cols;
//    if (h) *h = mat->rows;
//    if (chan) *chan = mat->channels();
//
//    return mat->data;
}

void checkGLError(const char *msg) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        LOGERR("%s - GL Error occured: %d", msg, err);
    }
}
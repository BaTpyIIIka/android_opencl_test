#include "cl_test.h"
#include <CL/cl_platform.h>

#include <assert.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>
#include <CL/cl.h>
#include <unistd.h>

static double tStart;
static double tFinish;
static char* kernel_name;
static cl_platform_id platform;
static cl_context context;
static cl_program  program;
static cl_kernel kernel;
static cl_int err;
static cl_device_id device;
static cl_sampler sampler;
static cl_command_queue queue;
static bool init_ok;

size_t global_ws[2];	// global work-size
size_t local_ws[2];		// local work-size
size_t max_work_group_size;

size_t img_w;
size_t img_h;
size_t img_bytes;
cl_mem mem_in;
cl_mem mem_out;

static double now_ms(void) {
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return 1000.0 * (double) res.tv_sec + (double) res.tv_nsec / 1e6;
}


float displayExecTime(const char *msg) {
    float ms = (float)(tFinish - tStart);
    LOGINFO("Measured time %s: %f ms\n", msg, ms);

    return ms;
}

bool gotError(const char *msg) {
    if (err != CL_SUCCESS) {
        if (msg != NULL) {
            LOGERR("CL Error %d occurred: %s", err, msg);
        } else {
            LOGERR("CL Error %d", err);
        }

        return true;
    }

    return false;
}


jint Java_ru_michaelilyin_alg1_CLUtils_initCL(JNIEnv *env, jobject obj)  {
    init_ok = false;

    kernel_name = NULL;

    // get platform
    clGetPlatformIDs(1, &platform, NULL);

    cl_context_properties contextProps[] = {
            CL_CONTEXT_PLATFORM,
            (cl_context_properties)platform,
            0
    };

    // create context
    context = clCreateContextFromType(contextProps,
                                      CL_DEVICE_TYPE_GPU,
                                      NULL, NULL, &err);

    if (gotError("CLBase: No OpenCL-capable GPU context found")) {
        return 1;
    }

    LOGINFO("Context created");

    // get device
    size_t deviceBufSize;
    err = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufSize);

    if (gotError(NULL) || deviceBufSize <= 0) {
        LOGERR("CLBase: Error finding OpenCL-GPU device");
        return 1;
    }

    LOGINFO("Found %lu devices", deviceBufSize / sizeof(cl_device_id));

    cl_device_id *devices = new cl_device_id[deviceBufSize / sizeof(cl_device_id)];

    err = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufSize, devices, NULL);

    if (gotError("CLBase: No OpenCL-GPU device ids")) {
        return 1;
    }

    LOGINFO("Got devices");

    device = devices[0];
    delete[] devices;

    init_ok = true;
    return 0;
}

bool createProg(const char *k_name, const char **prog_src, cl_uint src_count, const size_t *prog_src_bytes) {
    if (kernel_name) delete kernel_name;

    kernel_name = new char[strlen(k_name) + 1];
    strcpy(kernel_name, k_name);

    // Create the program
    program = clCreateProgramWithSource(context,
                                        src_count, prog_src,
                                        prog_src_bytes,
                                        &err);

    if (gotError("CLBase: Error creating OpenCL program")) {
        return false;
    }

    LOGINFO("Program created");

    // Build the program
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

    if (gotError(NULL)) {
        LOGERR("CLBase: Error building OpenCL program");

        if (program == NULL || device == NULL) {
            LOGERR("ERROR device or program is NULL");
        }

        char buildLog[16384];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
                              sizeof(buildLog), buildLog, NULL);

        LOGERR("CLBase: Build Log:");

        LOGERR("LOG: %s", buildLog);

        clReleaseProgram(program);

        return false;
    }

    return true;
}

jint Java_ru_michaelilyin_alg1_CLUtils_createProg(JNIEnv *env, jobject obj,
                                                       jstring progName,
                                                       jobject progSrcBuf) {
    // get the name
    char *clKernelProgName = (char *)(env->GetStringUTFChars(progName, NULL));

    // Get CL program source code
    char *progSrcFull = (char *)(env->GetDirectBufferAddress(progSrcBuf));
    const size_t progSizeFull = strlen(progSrcFull);

    LOGINFO("Creating OpenCL program with source length %lu", progSizeFull);
    LOGINFO("Full source:\n%s", progSrcFull);
    // CL requires to have source code parts of max. 1024 bytes length, so we need
    // to create these parts and devide "progSrcFull":
    const int SRC_MAX_BYTES = 1024;
    const unsigned int numSrcParts = progSizeFull / SRC_MAX_BYTES + 1;
    char **srcParts = new char*[numSrcParts];
    size_t *srcPartsLen = new size_t[numSrcParts];
    LOGINFO("Source parts = %d", numSrcParts);
    for (int i = 0; i < numSrcParts; i++) {
        // create memory space
        const size_t partLen = (i < numSrcParts - 1) ? SRC_MAX_BYTES : progSizeFull - i * SRC_MAX_BYTES;
        srcParts[i] 	= new char[partLen];
        srcPartsLen[i] 	= partLen;

        LOGINFO("Source part %d with str. length %lu", i, partLen);

        // copy substring
        strncpy(srcParts[i], progSrcFull + i * SRC_MAX_BYTES, partLen);

        LOGINFO("Source part data: %s", srcParts[i]);
    }
    // now create the program
    if (!createProg(clKernelProgName, (const char **)srcParts, numSrcParts, srcPartsLen)) {
        return 1;
    }

    LOGINFO("Program created");

    // now create the sampler
    // Create the sampler
    sampler = clCreateSampler(context, CL_FALSE,
                              CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_NEAREST,
                              &err);

    if (gotError("CLBase: Error creating OpenCL sampler")) {
        return 1;
    }

    return 0;
}


jint Java_ru_michaelilyin_alg1_CLUtils_createKernel(JNIEnv *env, jobject obj) {
    kernel = clCreateKernel(program, kernel_name, &err);

    if (gotError("CLBase: Error creating OpenCL kernel")) {
        LOGERR("CLBase: Kernel name: %s", kernel_name);
        return 1;
    }

    // get max. work-group size for the kernel program
    err = clGetKernelWorkGroupInfo(kernel, device,
                                   CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t),
                                   &max_work_group_size, NULL);

    LOGINFO("CLBase: Max. work group size for kernel: %lu", max_work_group_size);
    return 0;
}

jint Java_ru_michaelilyin_alg1_CLUtils_createCmdQueue(JNIEnv *env, jobject obj) {
    // Create a command queue
    queue = clCreateCommandQueue(context, device, 0, &err);

    if (gotError("CLBase: Error creating OpenCL command queue")) {
        return 1;
    }

    return 0;
}

jfloat Java_ru_michaelilyin_alg1_CLUtils_setKernelArgs(JNIEnv *env, jobject obj,
                                                            jint width, jint height,
                                                            jobject inputImgBuf) {

    // Get the image data
    void *inImg = (void *)(env->GetDirectBufferAddress(inputImgBuf));

    tStart = now_ms();
    img_w = width;
    img_h = height;

    LOGINFO("CLImgConv: Image size %lux%lu", img_w, img_h);

    img_bytes = img_w * img_h * 4;	// 4 channels!

    // Create the OpenCL image for the input image
    cl_image_format clImgFmt;
    clImgFmt.image_channel_order = CL_RGBA;
    clImgFmt.image_channel_data_type = CL_UNORM_INT8;

    if (mem_in) {
        clReleaseMemObject(mem_out);
    }

    if (mem_out) {
        clReleaseMemObject(mem_in);
    }

    mem_in = clCreateImage2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                             &clImgFmt, img_w, img_h,
                             0, inImg, &err);

    if (gotError("CLImgConv: Image input memory object")) {
        return false;
    }

    // Create the buffer for the output image
    mem_out = clCreateImage2D(context, CL_MEM_WRITE_ONLY,
                              &clImgFmt, img_w, img_h,
                              0, NULL, &err);

    if (gotError("CLImgConv: Image output memory object")) {
        return false;
    }

    // Set kernel arguments
    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &mem_in);
    if (gotError("set args: in")) { return -1.0f; }

    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &mem_out);
    if (gotError("set args: out")) { return -1.0f; }

    err |= clSetKernelArg(kernel, 2, sizeof(cl_sampler), &sampler);
    if (gotError("set args: sampler")) { return -1.0f; }

    err |= clSetKernelArg(kernel, 3, 4, &img_w);
    if (gotError("set args: width")) { return -1.0f; }

    err |= clSetKernelArg(kernel, 4, 4, &img_h);
    if (gotError("set args: height")) { return -1.0f; }

    tFinish = now_ms();
    float execTime = displayExecTime("input memory copy");

    return execTime;
}

size_t roundUp(int groupS, int globalS) {
    int r = globalS % groupS;
    if (r == 0) {
        return globalS;
    } else {
        return globalS + groupS - r;
    }
}

jfloat Java_ru_michaelilyin_alg1_CLUtils_executeKernel(JNIEnv *env, jobject obj) {
    size_t workSizePerDim = sqrt(max_work_group_size);		// Square root, because maxWorkGroupSize is for dim = 1, but we have dim = 2!
    local_ws[0] = local_ws[1] = 16;

    global_ws[0] = (int)ceil((float)img_w/16.0f)*16;
    global_ws[1] = (int)ceil((float)img_h/16.0f)*16;

    LOGINFO("CLImgConv: Worksizes: local - %lu, %lu | global - %lu, %lu",
            local_ws[0],
            local_ws[1],
            global_ws[0],
            global_ws[1]);

    // Execute the kernel
//    cl_event event;
    tStart = now_ms();
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL,
                                 global_ws, local_ws,
                                 0, NULL, NULL);
//    err |= clFlush(queue);
//#ifdef BENCHMARK
    err |= clFinish(queue);
//#endif
//    err |= clWaitForEvents(1, &event);
    if (gotError("CLBase: Error executing OpenCL kernel")) {
        return -1.0f;
    }

    tFinish = now_ms();
    float execTime = displayExecTime("kernel execution");

    return execTime;
}

jfloat Java_ru_michaelilyin_alg1_CLUtils_getResultImg(JNIEnv *env, jobject obj,
                                                           jobject outputImgBuf) {
    LOGINFO("Get image");
    void *outImg = (void *)(env->GetDirectBufferAddress(outputImgBuf));

    // Read the result
//    cl_event event;
    LOGINFO("Got put buffer");
    tStart = now_ms();
    size_t origin[] = {0,0,0};
    size_t region[] = {img_w,img_h,1};

    // Read the result
    err = clEnqueueReadImage(queue, mem_out, CL_TRUE,
                             origin, region, 0, 0,
                             outImg, 0, NULL, NULL);
//    err |= clFlush(queue);
//#ifdef BENCHMARK
    err |= clFinish(queue);
//#endif

    if (gotError("CLBase: Error reading result image")) {
        return -1.0f;
    }

    tFinish = now_ms();

    float execTime = displayExecTime("output memory copy");

    return execTime;
}

void Java_ru_michaelilyin_alg1_CLUtils_dealloc(JNIEnv *env, jobject obj) {
    clReleaseMemObject(mem_out);
    clReleaseMemObject(mem_in);
    clReleaseSampler(sampler);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);

    if (kernel_name) delete kernel_name;
}

void Java_ru_michaelilyin_alg1_CLUtils_printInfo(JNIEnv *env, jobject obj) {
	const cl_platform_info platInfoIds[] = {
			CL_PLATFORM_NAME,
			CL_PLATFORM_VENDOR,
			CL_PLATFORM_PROFILE,
			CL_PLATFORM_VERSION,
			CL_PLATFORM_EXTENSIONS
	};

	const char *platInfoStrings[] = {
			"CL_PLATFORM_NAME",
			"CL_PLATFORM_VENDOR",
			"CL_PLATFORM_PROFILE",
			"CL_PLATFORM_VERSION",
			"CL_PLATFORM_EXTENSIONS"
	};

	size_t items = sizeof(platInfoIds) / sizeof(platInfoIds[0]);

	for (int i = 0; i < items; ++i) {
		size_t s;
		clGetPlatformInfo(platform, platInfoIds[i], 0, NULL, &s);
		char *buf = new char[s];
		clGetPlatformInfo(platform, platInfoIds[i], s, buf, NULL);
		LOGINFO("%s: %s", platInfoStrings[i], buf);
		delete[] buf;
	}

	unsigned int devInfoOfTypeStr1 = 0;
	unsigned int devInfoOfTypeStr2 = 4;
	unsigned int devInfoOfTypeUInt1 = 5;
	unsigned int devInfoOfTypeUInt2 = 7;
	unsigned int devInfoOfTypeBool1 = 8;
	unsigned int devInfoOfTypeBool2 = 8;
	unsigned int devInfoOfTypeSizeT1 = 9;
	unsigned int devInfoOfTypeSizeT2 = 11;

	const cl_device_info devInfoIds[] = {
			CL_DEVICE_NAME,
			CL_DEVICE_VENDOR,
			CL_DRIVER_VERSION,
			CL_DEVICE_PROFILE,
			CL_DEVICE_VERSION,
			CL_DEVICE_MAX_COMPUTE_UNITS,
			CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
			CL_DEVICE_MAX_CLOCK_FREQUENCY,
			CL_DEVICE_IMAGE_SUPPORT,
			CL_DEVICE_MAX_WORK_GROUP_SIZE,
			CL_DEVICE_IMAGE2D_MAX_WIDTH,
			CL_DEVICE_IMAGE2D_MAX_HEIGHT
	};

	const char *devInfoStrings[] = {
			"CL_DEVICE_NAME",
			"CL_DEVICE_VENDOR",
			"CL_DRIVER_VERSION",
			"CL_DEVICE_PROFILE",
			"CL_DEVICE_VERSION",
			"CL_DEVICE_MAX_COMPUTE_UNITS",
			"CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS",
			"CL_DEVICE_MAX_CLOCK_FREQUENCY",
			"CL_DEVICE_IMAGE_SUPPORT",
			"CL_DEVICE_MAX_WORK_GROUP_SIZE",
			"CL_DEVICE_IMAGE2D_MAX_WIDTH",
			"CL_DEVICE_IMAGE2D_MAX_HEIGHT"
	};

	items = sizeof(devInfoIds) / sizeof(devInfoIds[0]);

	for (int i = 0; i < items; ++i) {
		size_t s;
		clGetDeviceInfo(device, devInfoIds[i], 0, NULL, &s);

		if (i >= devInfoOfTypeStr1 && i <= devInfoOfTypeStr2) {
			char *buf = new char[s];
			clGetDeviceInfo(device, devInfoIds[i], s, buf, NULL);
			LOGINFO("%s: %s", devInfoStrings[i], buf);
			delete[] buf;
		}

		if (i >= devInfoOfTypeUInt1 && i <= devInfoOfTypeUInt2) {
			cl_uint num;
			clGetDeviceInfo(device, devInfoIds[i], s, &num, NULL);
			LOGINFO("%s: %d", devInfoStrings[i], num);
		}

		if (i >= devInfoOfTypeBool1 && i <= devInfoOfTypeBool2) {
			cl_bool res;
			clGetDeviceInfo(device, devInfoIds[i], s, &res, NULL);
			LOGINFO("%s: %d", devInfoStrings[i], res);
		}

		if (i >= devInfoOfTypeSizeT1 && i <= devInfoOfTypeSizeT2) {
			size_t val;
			clGetDeviceInfo(device, devInfoIds[i], s, &val, NULL);
			LOGINFO("%s: %lu", devInfoStrings[i], val);
		}
	}

	// supported image formats
	cl_mem_flags suppImgFormatsMemFlags = CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR;
	cl_uint numImgFmts;
	err = clGetSupportedImageFormats(context, suppImgFormatsMemFlags,
									CL_MEM_OBJECT_IMAGE2D, 0, NULL,
									&numImgFmts);

    if (err != CL_SUCCESS) {
    	LOGERR("Error calling clGetSupportedImageFormats I");
    	return;
    }

    if (numImgFmts <= 0) {
    	LOGINFO("No image formats supported.");
    	return;
    }

    cl_image_format *imgFmts = new cl_image_format[numImgFmts];
	err = clGetSupportedImageFormats(context, suppImgFormatsMemFlags,
									CL_MEM_OBJECT_IMAGE2D, numImgFmts, imgFmts,
									NULL);
    if (err != CL_SUCCESS) {
    	LOGERR("Error calling clGetSupportedImageFormats II");
    	return;
    }

    for (cl_uint i = 0; i < numImgFmts; i++) {
    	cl_image_format fmt = imgFmts[i];
    	LOGINFO("Supported image format %d: order %04x, type %04x", i, fmt.image_channel_order, fmt.image_channel_data_type);
    }

}


void Java_ru_michaelilyin_alg1_CLUtils_begin(JNIEnv *env, jobject obj) {
}

jfloat Java_ru_michaelilyin_alg1_CLUtils_end(JNIEnv *env, jobject obj) {
    double s = now_ms();
    sleep(1);
    double e = now_ms();
    return (float)(e - s);
}





















//#include <GLES2/gl2.h>
//#include "gl_gpgpu/view.h"
//#include "gl_gpgpu/pipeline.h"
//
//static Pipeline *pipeline = NULL;
//static View *view = NULL;
//static bool glContextCreated = false;
//
//void Java_ru_michaelilyin_alg1_gl_ImgprocJNI_initGL(JNIEnv *env, jobject obj) {
//    pipeline = new Pipeline();
//    view = new View(pipeline);
//}
//
//void Java_ru_michaelilyin_alg1_gl_ImgprocJNI_glviewCreate(JNIEnv *env, jobject obj) {
//    LOGINFO("glview_create");
//
//    view->create();
//
//    glContextCreated = true;
//}
//
//void Java_ru_michaelilyin_alg1_gl_ImgprocJNI_glviewResize(JNIEnv *env, jobject obj, jint w, jint h) {
//    LOGINFO("glview_resize to %dx%d", w ,h);
//
//    view->resize(w, h);
//}
//
//void Java_ru_michaelilyin_alg1_gl_ImgprocJNI_glviewDraw(JNIEnv *env, jobject obj) {
//    view->draw();
//}
//
//void Java_ru_michaelilyin_alg1_gl_ImgprocJNI_setInputImage(JNIEnv *env, jobject obj, jlong inputImgAddr) {
//    if (!glContextCreated) {
//        LOGERR("set_input_image requires a GL context!");
//        return;
//    }
//
//    // get the pointer to the image data
//    int imgW, imgH, imgChan;
//    unsigned char *imgData = imgDataPtrFromCvMatPtrAddr(inputImgAddr, &imgW, &imgH, &imgChan);
//
//    LOGINFO("set_input_image: got image of size %dx%d with %d channels", imgW, imgH, imgChan);
//
//    // upload it to the GPU
//    pipeline->setInputImageFromBytes(imgData, imgW, imgH);
//}
//
//void Java_ru_michaelilyin_alg1_gl_ImgprocJNI_getOutputImage(JNIEnv *env, jobject obj, jlong outputImgAddr) {
//    if (!glContextCreated) {
//        LOGERR("get_output_image requires a GL context!");
//        return;
//    }
//
//    // get the pointer to the image data
//    int imgW, imgH, imgChan;
//    unsigned char *imgData = imgDataPtrFromCvMatPtrAddr(outputImgAddr, &imgW, &imgH, &imgChan);
//
//    LOGINFO("get_output_image: got image of size %dx%d with %d channels", imgW, imgH, imgChan);
//
//    // read it back from the GPU
//    pipeline->copyOutputImageToPointer(imgData, imgW, imgH);
//}
//
//void Java_ru_michaelilyin_alg1_gl_ImgprocJNI_cleanup(JNIEnv *env, jobject obj) {
//    LOGINFO("cleanup");
//
//    if (view) delete view;
//    if (pipeline) delete pipeline;
//}
//
//jfloat Java_ru_michaelilyin_alg1_gl_ImgprocJNI_getImagePushMs(JNIEnv *env, jobject obj) {
//#ifdef BENCHMARK
//    return pipeline->getImagePushMs();
//#else
//    return 0.0f;
//#endif
//}
//jfloat Java_ru_michaelilyin_alg1_gl_ImgprocJNI_getRenderMs(JNIEnv *env, jobject obj) {
//#ifdef BENCHMARK
//    return pipeline->getRenderMs();
//#else
//    return 0.0f;
//#endif
//}
//jfloat Java_ru_michaelilyin_alg1_gl_ImgprocJNI_getImagePullMs(JNIEnv *env, jobject obj) {
//#ifdef BENCHMARK
//    return pipeline->getImagePullMs();
//#else
//    return 0.0f;
//#endif
//}
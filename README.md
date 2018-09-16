Exsample how to run OpenCL on Android devices

1) Tested on:
  - Ubuntu 14.04;
  - Android Studio 3.1.2; 
  - NDK: r17; 
  - Compile SDK: API 24: 
  - Android 7.0 (Nougat); 
  - Gradle 3.3
  
  can be easily build on macOs
  
2) Android Studio -> import project, gradle
 
3) Go to android_opencl_test/app/src/main/cpp/CMakeLists.txt 
 
4) Replace 
set_target_properties(lib_ocl PROPERTIES IMPORTED_LOCATION /system/vendor/lib64/libPVROCL.so) 
with ABSOLUTE path to any libPVROCL.so in THIS project, e.g
set_target_properties(lib_ocl PROPERTIES IMPORTED_LOCATION /home/user/android_opencl_test/app/src/main/cpp/libs/arm64-v8a/libPVROCL.so)

5) Build 

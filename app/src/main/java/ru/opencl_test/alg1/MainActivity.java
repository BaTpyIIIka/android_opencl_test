package ru.opencl_test.alg1;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.opengl.GLSurfaceView;
import android.os.AsyncTask;
import android.os.Bundle;
import android.renderscript.Allocation;
import android.renderscript.RenderScript;
import android.util.Log;
import android.widget.ImageView;
import android.widget.LinearLayout;

import java.nio.ByteBuffer;
import java.util.ArrayList;

import ru.michaelilyin.alg1.R;
import ru.michaelilyin.alg1.ScriptC_gauss_for_5x5;
import ru.michaelilyin.alg1.ScriptC_gauss_for_5x5_fs;
import ru.opencl_test.alg1.gl.GLRendererSimple;


public class MainActivity extends Activity {

    private static final String TAG = "MainActivity";

    private LinearLayout lt;

    private class ImagePair {
        String name;
        Bitmap image;

        public ImagePair(String name, int image) {
            this.name = name;
            this.image = BitmapFactory.decodeResource(getResources(), image);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        lt = (LinearLayout) findViewById(R.id.lt);

        test();

//        final GLSurfaceView view = new GLSurfaceView(this);
//        final GLRendererSimple renderer = new GLRendererSimple(this);
//        view.setEGLContextClientVersion(2);
//        view.setRenderer(renderer);
//        view.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
//        setContentView(view);

        new AsyncTask<Void, Void, Void>() {
            @Override
            protected Void doInBackground(Void... params) {
                ArrayList<ImagePair> images = new ArrayList<>();
                images.add(new ImagePair("128", R.mipmap.p128));
                images.add(new ImagePair("256", R.mipmap.p256));
                images.add(new ImagePair("512", R.mipmap.p512));
                images.add(new ImagePair("1024", R.mipmap.p1024));
                images.add(new ImagePair("2048", R.mipmap.p2048));
//                images.add(new ImagePair("4096", R.mipmap.p4096));

                long start;
                long stop;
                float duration;

                RenderScript rs = initRS();
                initCL();
                try {
                    for (ImagePair image : images) {
                        Log.e(TAG, "Test for " + image.name);

                        Log.i(TAG, "Test RS");
                        duration = testRS(rs, image);
                        Log.i(TAG, "Test RS completed: " + duration);
//
                        Log.i(TAG, "Test FS");
                        duration = testFS(rs, image);
                        Log.i(TAG, "Test FS completed: " + duration);

//                        Log.i(TAG, "Test OGL");
//                        duration = testOGL(renderer, view, image);
//                        Log.i(TAG, "Test OGL completed: " + duration);

                        Log.i(TAG, "Test OCL");
                        duration = testOCL(image);
                        Log.i(TAG, "Test OCL completed: " + duration);
//
                        Log.e(TAG, "Test for " + image.name + " completed");
//                        Thread.sleep(1000);
                    }
//                } catch (InterruptedException e) {
//                    e.printStackTrace();
                } finally {
                    CLUtils.dealloc();
                }

                return null;
            }
        }.execute((Void) null);
    }

    private float testOGL(GLRendererSimple renderer, GLSurfaceView view, ImagePair image) {
        int depth = 10;

        final Object monitor = new Object();

//        for (int test = 0; test < depth; test++) {
            renderer.test(view, image.image, depth, new GLRendererSimple.Listener() {
                @Override
                public void onFinish(float pushMsAvg, float execMsAvg, float pullMsAvg) {
                    Log.w(TAG, "ogl push: " + pushMsAvg);
                    Log.w(TAG, "ogl exec: " + execMsAvg);
                    Log.w(TAG, "ogl pull: " + pullMsAvg);
                    synchronized (monitor) {
                        monitor.notifyAll();
                    }
                }
            });
            synchronized (monitor) {
                try {
                    monitor.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
//        }

//        Bitmap outputBitmap = Bitmap.createBitmap(imgWidth, imgHeight, Bitmap.Config.ARGB_8888);
//        outputBitmap.copyPixelsFromBuffer(outputImgBuf);
//        showRes(outputBitmap);

        return 0;
    }

    private void test() {
        float dur = CLUtils.end();

        float st = System.nanoTime();
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        float en = System.nanoTime();

        float dur1 = (en - st) / 1000000;

        Log.wtf(TAG, String.format("Time test: C++: %f, Java: %f", dur, dur1));
//        throw new RuntimeException();

    }

    private void initCL() {
        if (CLUtils.initCL() == 0) {
            Log.i(TAG, "CL init successful");
        } else {
            Log.e(TAG, "CL init error");
        }

        CLUtils.printInfo();

        ByteBuffer progSrc;

            Log.i(TAG, "Loading program source code in file " + "raw/gauss_5x5.cl");
            progSrc = CLUtils.getProgramBuffer(getBaseContext(), "raw/gauss_5x5.cl");
            Log.i(TAG, "Loaded CL program source code containing " + progSrc.capacity() + " bytes");

//            progSrc = ByteBuffer.allocateDirect(clKernelProgSrc.length());
//            progSrc.put(clKernelProgSrc.getBytes());
//            progSrc.rewind();
//            Log.i(TAG, "Using generated kernel source of size " + progSrc.capacity() + " bytes");
//        }

        if (CLUtils.createProg("cl_filter", progSrc) == 0) {
            Log.i(TAG, "CL createProg successful");
        } else {
            Log.e(TAG, "CL createProg error");
        }

        if (CLUtils.createKernel() == 0) {
            Log.i(TAG, "CL createKernel successful");
        } else {
            Log.e(TAG, "CL createKernel error");
        }

        if (CLUtils.createCmdQueue() == 0) {
            Log.i(TAG, "CL createCmdQueue successful");
        } else {
            Log.e(TAG, "CL createCmdQueue error");
        }
    }

    private RenderScript initRS() {
        return RenderScript.create(MainActivity.this);
    }

    private float testOCL(ImagePair image) {
        int imgWidth = image.image.getWidth();
        int imgHeight = image.image.getHeight();
        long start;
        long stop;
        float duration = 0;
        Log.i(TAG, "Java image size: " + imgWidth + "x" + imgHeight);

        int imgChan = image.image.getRowBytes() / imgWidth;
        int imgByteSize = imgWidth * imgHeight * imgChan;
        ByteBuffer outputImgBuf = null;

        Log.i(TAG, "Chan size " + imgChan);

        ByteBuffer inputImgBuf = ByteBuffer.allocateDirect(imgByteSize);
        image.image.copyPixelsToBuffer(inputImgBuf);
        inputImgBuf.rewind();

        // allocate output buffer
        outputImgBuf = ByteBuffer.allocateDirect(imgByteSize);

        float pushMsSum = 0.0f;
        float execMsSum = 0.0f;
        float pullMsSum = 0.0f;
        int depth = 100;
        for (int test = 0; test < depth; test++) {
            float pushMs = CLUtils.setKernelArgs(imgWidth, imgHeight, inputImgBuf);
            if (pushMs < 0.0f) {
                Log.e(TAG, "CL setKernelArgs error");
                return -1;
            }
            pushMsSum += pushMs;

            start = System.nanoTime();
            float execMs = CLUtils.executeKernel();
            stop = System.nanoTime();
            if (execMs < 0.0f) {
                Log.e(TAG, "CL executeKernel error");
                return -1;
            }
//            try {
//                Thread.sleep(300000);
//            } catch (InterruptedException e) {
//                e.printStackTrace();
//            }
            duration += (float)((double)(stop - start));
            execMsSum += execMs;
            float pullMs = CLUtils.getResultImg(outputImgBuf);
            outputImgBuf.rewind();
            if (pullMs < 0.0f)  {
                Log.e(TAG, "CL getResultImg error");
                return -1;
            }
            pullMsSum += pullMs;
        }

        float pushMsAvg = pushMsSum / (float)depth;
        float execMsAvg = execMsSum / (float)depth;
        float pullMsAvg = pullMsSum / (float)depth;
        float dur = duration / depth / 1000000;

        Log.w(TAG, "ocl push: " + pushMsAvg);
        Log.w(TAG, "ocl exec: " + execMsAvg);
        Log.w(TAG, "ocl pull: " + pullMsAvg);

        Bitmap outputBitmap = Bitmap.createBitmap(imgWidth, imgHeight, Bitmap.Config.ARGB_8888);
        outputBitmap.copyPixelsFromBuffer(outputImgBuf);
        showRes(outputBitmap);

        return pushMsAvg + execMsAvg + pullMsAvg;
    }

    private float testRS(RenderScript rs, final ImagePair image) {
        long start;
        long stop;
        float duration = 0;
        float durIn = 0;
        float durOut = 0;
        Bitmap outBm = Bitmap.createBitmap(image.image.getWidth(),
                image.image.getHeight(), image.image.getConfig());

        ScriptC_gauss_for_5x5 gauss = new ScriptC_gauss_for_5x5(rs);
        Log.i(TAG, "Java image size: " + image.image.getWidth() + "x" + image.image.getHeight());
        int depth = 2;
        for (int i = 0; i < depth; i++) {
            rs.finish();
            start = System.nanoTime();
            Allocation in = Allocation.createFromBitmap(rs, image.image,
                    Allocation.MipmapControl.MIPMAP_NONE,
                    Allocation.USAGE_SCRIPT);
            stop = System.nanoTime();

            durIn += (float)((double)(stop - start));


            Allocation out = Allocation.createFromBitmap(rs, outBm,
                    Allocation.MipmapControl.MIPMAP_NONE,
                    Allocation.USAGE_SCRIPT);

            start = System.nanoTime();
            gauss.set_in(in);
            gauss.set_size(image.image.getWidth());
            in.syncAll(Allocation.USAGE_SCRIPT);
            rs.finish();
            stop = System.nanoTime();
            durIn += (float)((double)(stop - start));

            start = System.nanoTime();
            gauss.forEach_make_gauss(out);
            rs.finish();
            stop = System.nanoTime();
            duration += (float)((double)(stop - start));

            start = System.nanoTime();
            out.syncAll(Allocation.USAGE_SCRIPT);
            out.copyTo(outBm);
            stop = System.nanoTime();
            durOut += (float)((double)(stop - start));
//            in = Allocation.createFromBitmap(rs, image.image,
//                    Allocation.MipmapControl.MIPMAP_NONE,
//                    Allocation.USAGE_SCRIPT);
        }
        showRes(outBm);

        float pushMsAvg = durIn / (float)depth / 1000000;
        float execMsAvg = duration / (float)depth / 1000000;
        float pullMsAvg = durOut / (float)depth / 1000000;

        Log.w(TAG, "rs push: " + pushMsAvg);
        Log.w(TAG, "rs exec: " + execMsAvg);
        Log.w(TAG, "rs pull: " + pullMsAvg);

        return pushMsAvg + execMsAvg + pullMsAvg;
    }

    private float testFS(RenderScript rs, final ImagePair image) {
        long start;
        long stop;
        float duration = 0;
        float durIn = 0;
        float durOut = 0;
        Bitmap outBm = Bitmap.createBitmap(image.image.getWidth(),
                image.image.getHeight(), image.image.getConfig());

        ScriptC_gauss_for_5x5_fs gauss = new ScriptC_gauss_for_5x5_fs(rs);
        Log.i(TAG, "Java image size: " + image.image.getWidth() + "x" + image.image.getHeight());
        int depth = 2;
        for (int i = 0; i < depth; i++) {
            rs.finish();

            start = System.nanoTime();
            Allocation in = Allocation.createFromBitmap(rs, image.image,
                    Allocation.MipmapControl.MIPMAP_NONE,
                    Allocation.USAGE_SCRIPT);
            stop = System.nanoTime();

            durIn += (float)((double)(stop - start));


            Allocation out = Allocation.createFromBitmap(rs, outBm,
                    Allocation.MipmapControl.MIPMAP_NONE,
                    Allocation.USAGE_SCRIPT);

            start = System.nanoTime();
            gauss.set_in(in);
            gauss.set_size(image.image.getWidth());
            in.syncAll(Allocation.USAGE_SCRIPT);
            rs.finish();
            stop = System.nanoTime();
            durIn += (float)((double)(stop - start));

            start = System.nanoTime();
            gauss.forEach_make_gauss(out);
            rs.finish();
            stop = System.nanoTime();
            duration += (float)((double)(stop - start));

            start = System.nanoTime();
            out.syncAll(Allocation.USAGE_SCRIPT);
            out.copyTo(outBm);
            stop = System.nanoTime();
            durOut += (float)((double)(stop - start));
//            in = Allocation.createFromBitmap(rs, image.image,
//                    Allocation.MipmapControl.MIPMAP_NONE,
//                    Allocation.USAGE_SCRIPT);
        }
        showRes(outBm);

        float pushMsAvg = durIn / (float)depth / 1000000;
        float execMsAvg = duration / (float)depth / 1000000;
        float pullMsAvg = durOut / (float)depth / 1000000;

        Log.w(TAG, "fs push: " + pushMsAvg);
        Log.w(TAG, "fs exec: " + execMsAvg);
        Log.w(TAG, "fs pull: " + pullMsAvg);

        return pushMsAvg + execMsAvg + pullMsAvg;
    }

    private void showRes(final Bitmap bitmap) {
        this.lt.post(new Runnable() {
            @Override
            public void run() {
                LinearLayout layout = new LinearLayout(MainActivity.this);
                layout.setOrientation(LinearLayout.HORIZONTAL);

                {
//                    ImageView imageView = new ImageView(MainActivity.this);
    //                BitmapDrawable bitmapDrawable = new BitmapDrawable(getResources(), image.image);
    //                imageView.setImageDrawable(bitmapDrawable);
    //                layout.addView(imageView);
                }
                {
                    ImageView imageView = new ImageView(MainActivity.this);
                    BitmapDrawable bitmapDrawable = new BitmapDrawable(getResources(), bitmap);
                    imageView.setImageDrawable(bitmapDrawable);
                    layout.addView(imageView, new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT));
                }

                lt.addView(layout, new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT));
            }
        });
    }

}

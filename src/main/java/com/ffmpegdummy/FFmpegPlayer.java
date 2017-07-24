package com.ffmpegdummy;

import android.view.Surface;
import android.view.SurfaceHolder;

/**
 * Created by yw on 2017/7/20.
 */

public class FFmpegPlayer {


    public static native VideoInfo openVideo(String mediaPath);

   // public static

    public static native void play(Surface surface);

   // public static native void pause();

    public static native void release();


}

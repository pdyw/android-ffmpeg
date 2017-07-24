package com.ffmpegdummy;

import android.content.Context;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewGroup;
import android.widget.VideoView;

import java.io.File;
import java.util.logging.Logger;

/**
 * Created by yw on 2017/7/20.
 */

public class VideoSurfaceView extends SurfaceView implements SurfaceHolder.Callback {

    VideoInfo info;
    Handler uiHandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(Message msg) {
            int width = getResources().getDisplayMetrics().widthPixels;
            int height = info.height * width / info.width;
            ViewGroup.LayoutParams lp = VideoSurfaceView.this.getLayoutParams();
            lp.width = width;
            lp.height = height;
            VideoSurfaceView.this.setLayoutParams(lp);
        }
    };

    public VideoSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    public VideoSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public VideoSurfaceView(Context context) {
        super(context);
        init();
    }

    private void init() {
        SurfaceHolder holder = this.getHolder();
        holder.addCallback(this);
    }

    @Override
    public void surfaceCreated(final SurfaceHolder holder) {


        new Thread(new Runnable() {
            @Override
            public void run() {
                final String media = Environment.getExternalStorageDirectory().getPath() + "/UCDownloads/VideoData/dance.mp4";
                info = FFmpegPlayer.openVideo(media);
                if (info != null) {
                    uiHandler.sendEmptyMessage(0x01);
                    FFmpegPlayer.play(VideoSurfaceView.this.getHolder().getSurface());
                }
            }
        }).start();


    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        FFmpegPlayer.release();
        this.getHolder().removeCallback(this);

    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);

    }
}

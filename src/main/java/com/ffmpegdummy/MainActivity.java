package com.ffmpegdummy;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;

import java.io.File;

public class MainActivity extends AppCompatActivity {


    static {
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("swscale-4");
        System.loadLibrary("native-lib");
    }

    VideoSurfaceView videoSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        videoSurfaceView = (VideoSurfaceView) findViewById(R.id.video_surface);

        int per = ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE);
        if (per != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 103);

        } else {
            doffmpeg();
        }


    }

    void doffmpeg() {
        videoSurfaceView.setVisibility(View.VISIBLE);


    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        for (int i = 0; i < permissions.length; i++) {
            String per = permissions[i];
            if (per.equals(Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
                if (grantResults[i] == PackageManager.PERMISSION_GRANTED) {
                    doffmpeg();
                    break;
                }
            }
        }

    }
}

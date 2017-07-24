//
// Created by yw on 2017/7/24.
//
#include <jni.h>
#include "player.h"
#include "logger.h"

#include <thread>
#include <mutex>


static FFmpegPlayer mPlayer;

static bool quit;


#ifdef __cplusplus
extern "C" {
#endif

ANativeWindow *nativeWindow;
ANativeWindow_Buffer window_buffer;

JNIEXPORT jobject JNICALL Java_com_ffmpegdummy_FFmpegPlayer_openVideo(JNIEnv *env, jobject obj,
                                                                      jstring videoPath) {

    const char *path = env->GetStringUTFChars(videoPath, JNI_FALSE);
    if (!path)
        return NULL;

    bool canOpen = mPlayer.Open(path);
    env->ReleaseStringUTFChars(videoPath, path);


    if (canOpen) {
        jclass info_clz = env->FindClass("com/ffmpegdummy/VideoInfo");

        if (!info_clz)
            return NULL;


        jmethodID m_init = env->GetMethodID(info_clz, "<init>", "()V");

        jfieldID f_width = env->GetFieldID(info_clz, "width", "I");
        jfieldID f_height = env->GetFieldID(info_clz, "height", "I");
        jfieldID f_fps = env->GetFieldID(info_clz, "fps", "I");
        jfieldID f_totalMs = env->GetFieldID(info_clz, "totalMs", "I");


        jobject info = env->NewObject(info_clz, m_init);
        env->SetIntField(info, f_width, mPlayer.videoWidth);
        env->SetIntField(info, f_height, mPlayer.videoHeight);
        env->SetIntField(info, f_fps, mPlayer.fps);
        env->SetIntField(info, f_totalMs, mPlayer.totalMs);

        return info;
    }

    else
        return NULL;
}

void runDecoder() {

    while (!quit && mPlayer.canDecode) {
        mPlayer.Decode();
        std::chrono::milliseconds dura(2);
        std::this_thread::sleep_for(dura);

    }
}

void runDisplay() {

    while (!quit && mPlayer.canDecode) {
        mPlayer.Display(nativeWindow,&window_buffer);
        std::chrono::milliseconds dura(24);
        std::this_thread::sleep_for(dura);

    }
}

JNIEXPORT void JNICALL Java_com_ffmpegdummy_FFmpegPlayer_play(JNIEnv *env, jobject obj,
                                                              jobject surface) {
    quit = false;

    nativeWindow = ANativeWindow_fromSurface(env, surface);
    ANativeWindow_setBuffersGeometry(nativeWindow, mPlayer.videoWidth, mPlayer.videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);

    std::thread decodeThread(runDecoder);
    std::thread displayThread(runDisplay);

    decodeThread.join();
    displayThread.join();


}

JNIEXPORT void JNICALL Java_com_ffmpegdummy_FFmpegPlayer_release(JNIEnv *env, jobject obj) {
    quit = true;
    if(!nativeWindow)
        ANativeWindow_release(nativeWindow);
    mPlayer.Close();

}


#ifdef __cplusplus
}
#endif


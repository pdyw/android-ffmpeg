//
// Created by yw on 2017/7/24.
//

#ifndef OPENCV4ANDROIDWITHCMAKE_MASTER_PLAYER_H
#define OPENCV4ANDROIDWITHCMAKE_MASTER_PLAYER_H
extern "C"{
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
};

class FFmpegPlayer {
public:
    AVFormatContext *ic;
    AVFrame *yuv;
    AVFrame *rgbFrame;
    AVCodecContext *videoCtx;
    bool canDecode=true;

    SwsContext *swsCtx;

    int fps;
    int totalMs;
    int videoStream;
    int videoWidth;
    int videoHeight;

    //method
    bool Open(const char *path);
    void Close();

    void Decode();

    void Display(ANativeWindow* window, ANativeWindow_Buffer* outBuffer);





};

#endif //OPENCV4ANDROIDWITHCMAKE_MASTER_PLAYER_H

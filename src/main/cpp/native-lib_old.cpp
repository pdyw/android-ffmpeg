#include <jni.h>
#include <android/log.h>
#include <string>


#define  LOG_TAG    "appdebug"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG, __VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)

extern "C" {
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>


JNIEXPORT void JNICALL Java_com_ffmpegdummy_FFmpegUtils_init
        (JNIEnv *env, jclass obj) {


}
JNIEXPORT void JNICALL Java_com_ffmpegdummy_FFmpegUtils_play
        (JNIEnv *env, jclass obj, jstring input, jobject surface) {
    const char *path = env->GetStringUTFChars(input, JNI_FALSE);
    av_register_all();
    AVFormatContext *ic = NULL;
    int re = avformat_open_input(&ic, path, 0, 0);
    if (re != 0) {
        return;

    }


    int videoStream = 0;
    AVCodecContext *videoCtx = NULL;
    for (int i = 0; i < ic->nb_streams; i++) {
        AVCodecContext *enc = ic->streams[i]->codec;
        if (enc->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            videoCtx = enc;
            AVCodec *codec = avcodec_find_decoder(enc->codec_id);
            if (!codec) {
                return;

            }
            int err = avcodec_open2(enc, codec, NULL);
            if (err != 0) {
                return;
            }
        }
    }
    //压缩过的视频帧
    AVFrame *yuv = av_frame_alloc();
    AVFrame *rgbFrame=av_frame_alloc();

    SwsContext *swsCtx = NULL;//解码器 yuv to rgb
    int outWidth = 640;
    int outHeight = 480;
    /*
     *  int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, videoCtx->width, videoCtx->height, 1);
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, AV_PIX_FMT_RGBA,
                         videoCtx->width, videoCtx->height, 1);
     */
    int numBytes=av_image_get_buffer_size(AV_PIX_FMT_RGBA,outWidth,outHeight,1);
    uint8_t  *buffer=(uint8_t*)av_malloc(numBytes*sizeof(uint8_t));
    av_image_fill_arrays(rgbFrame->data,rgbFrame->linesize,buffer,AV_PIX_FMT_RGBA,outWidth,outHeight,1);

    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    ANativeWindow_setBuffersGeometry(nativeWindow, outWidth, outHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer window_buffer;

    char *rgb = new char[outWidth * outHeight * 4];

    for (; ;) {
        AVPacket pkt;
        re = av_read_frame(ic, &pkt);
        if (re != 0)
            break;
        if (pkt.stream_index != videoStream) {
            av_packet_unref(&pkt);
            continue;
        }


        int vre = avcodec_send_packet(videoCtx, &pkt);
        if (vre != 0) {
            av_packet_unref(&pkt);
            continue;

        }

        vre = avcodec_receive_frame(videoCtx, yuv);
        if (vre != 0) {
            av_packet_unref(&pkt);
            continue;

        }

        swsCtx = sws_getCachedContext(swsCtx, videoCtx->width, videoCtx->height, videoCtx->pix_fmt,
                                      outWidth, outHeight, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL,
                                      NULL);
        if (!swsCtx) {
            LOGE("swsctx fail");
            av_packet_unref(&pkt);
            break;
        }

       /* uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
        data[0] = (uint8_t *) rgb;
        int linesize[AV_NUM_DATA_POINTERS] = {0};
        linesize[0] = outWidth * 4;*/

        ANativeWindow_lock(nativeWindow, &window_buffer, 0);
        sws_scale(swsCtx, (uint8_t const *const *) yuv->data, yuv->linesize, 0,
                  videoCtx->height, rgbFrame->data, rgbFrame->linesize);


        int dstStride = window_buffer.stride * 4;
        uint8_t *src = (uint8_t *) (rgbFrame->data[0]);
        int srcStride = rgbFrame->linesize[0];

        uint8_t *dst = (uint8_t *) window_buffer.bits;

        // 由于window的stride和帧的stride不同,因此需要逐行复制
        for (int row = 0; row < outHeight; row++) {
            memcpy(dst + row * dstStride, src + row * srcStride, srcStride);
        }

        // LOGE("%d\n", h);

        ANativeWindow_unlockAndPost(nativeWindow);

        av_packet_unref(&pkt);

    }
    env->ReleaseStringUTFChars(input, path);
    ANativeWindow_release(nativeWindow);
    if (swsCtx) {
        sws_freeContext(swsCtx);
        swsCtx = NULL;
    }

    av_free(buffer);
    av_frame_free(&yuv);
    av_frame_free(&rgbFrame);
    avcodec_close(videoCtx);
    avformat_close_input(&ic);
    ic = NULL;

}


JNIEXPORT jstring JNICALL Java_com_ffmpegdummy_FFmpegUtils_decode
        (JNIEnv *env, jclass obj, jstring str, jobject surface) {

    const char *path;
    path = env->GetStringUTFChars(str, JNI_FALSE);
    if (!path) {
        LOGE(" get null %s", "c++");
        return NULL;
    }
    av_register_all();

    AVFormatContext *ic = NULL;
    int re = avformat_open_input(&ic, path, 0, 0);
    if (re != 0) {
        return NULL;
    }
    int totalSec = ic->duration / AV_TIME_BASE;
    // LOGI("total  second%ld", totalSec);

    int videoStream;
    //获取码类型
    AVCodecContext *videoCtx = NULL;
    for (int i = 0; i < ic->nb_streams; i++) {

        AVCodecContext *ctx = ic->streams[i]->codec;
        if (ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
            videoStream = i;
            videoCtx = ctx;
            //查找h264
            AVCodec *codec = avcodec_find_decoder(ctx->codec_id);
            if (!codec)
                return NULL;
            //能否打开
            int err = avcodec_open2(ctx, codec, NULL);
            if (err != 0)
                return NULL;
        }

    }
    //解码
    //AVPacket
    AVFrame *yuv = av_frame_alloc();
    AVFrame *rgbFrame = av_frame_alloc();

    SwsContext *swsCtx = NULL;

    int videoWidth = videoCtx->width;
    int videoHeight = videoCtx->height;


    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, videoCtx->width, videoCtx->height, 1);
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, AV_PIX_FMT_RGBA,
                         videoCtx->width, videoCtx->height, 1);


    // ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    /*  ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                       WINDOW_FORMAT_RGBA_8888);
      ANativeWindow_Buffer window_buffer;*/

    for (; ;) {
        AVPacket pkt;
        int result = av_read_frame(ic, &pkt);
        if (result != 0)
            break;
        if (pkt.stream_index != videoStream) {
            av_packet_unref(&pkt);
            continue;
        }

        result = avcodec_send_packet(videoCtx, &pkt);
        if (result != 0) {
            av_packet_unref(&pkt);
            continue;
        }

        result = avcodec_receive_frame(videoCtx, yuv);
        if (result != 0) {
            av_packet_unref(&pkt);
            continue;
        }


        swsCtx = sws_getCachedContext(swsCtx, videoWidth, videoHeight, videoCtx->pix_fmt,
                                      videoWidth, videoHeight, AV_PIX_FMT_RGBA, SWS_BICUBIC,
                                      NULL, NULL, NULL);


        if (!swsCtx) {
            av_packet_unref(&pkt);
            //    av_frame_free(&yuv);
            continue;
        }
        // ANativeWindow_lock(nativeWindow, &window_buffer, 0);
        // 格式转换
        /*sws_scale(swsCtx, (uint8_t const *const *) yuv->data,
                  yuv->linesize, 0, yuv->height,
                  rgbFrame->data, rgbFrame->linesize);*/

        /* int dstStride = window_buffer.stride * 4;
         uint8_t *src = (uint8_t *) (rgbFrame->data[0]);
         int srcStride = rgbFrame->linesize[0];

         uint8_t *dst=(uint8_t*)window_buffer.bits;

         // 由于window的stride和帧的stride不同,因此需要逐行复制
         int h;
         for (h = 0; h < videoHeight; h++) {
             memcpy(dst + h * dstStride, src + h * srcStride, srcStride);


         }*/

        // ANativeWindow_unlockAndPost(nativeWindow);
        av_packet_unref(&pkt);

    }


    env->ReleaseStringUTFChars(str, path);
    // ANativeWindow_release(nativeWindow);

    av_frame_free(&rgbFrame);
    av_frame_free(&yuv);

    avcodec_close(videoCtx);
    avformat_close_input(&ic);
    return NULL;


}
}





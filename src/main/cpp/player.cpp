//
// Created by yw on 2017/7/24.
//

#include "player.h"
#include "logger.h"
#include <mutex>

std::mutex mMutex;

static double r2d(AVRational r) {
    return r.num == 0 || r.den == 0 ? 0 : (double) r.num / (double) r.den;
}

bool FFmpegPlayer::Open(const char *path) {
    Close();
    av_register_all();

    int re = avformat_open_input(&ic, path, 0, 0);
    if (re != 0)
        return false;

    for (int i = 0; i < ic->nb_streams; i++) {
        AVCodecContext *ctx = ic->streams[i]->codec;
        if (ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            videoCtx = ctx;
            AVCodec *codec = avcodec_find_decoder(ctx->codec_id);
            if (!codec)
                return false;
            re = avcodec_open2(ctx, codec, NULL);
            if (re != 0)
                return false;
        }
    }

    yuv = av_frame_alloc();
    rgbFrame = av_frame_alloc();

    totalMs = 1000 * ic->duration / AV_TIME_BASE;
    fps = r2d(ic->streams[videoStream]->avg_frame_rate);
    videoWidth = videoCtx->width;
    videoHeight = videoCtx->height;

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, videoWidth, videoHeight, 1);
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, AV_PIX_FMT_RGBA, videoWidth,
                         videoHeight, 1);



    LOGI("totalMs:%d,fps:%d,width:%d,height:%d", totalMs, fps, videoWidth, videoHeight);


    return true;
}

void FFmpegPlayer::Decode() {
    mMutex.lock();
    AVPacket pkt;
    int re = av_read_frame(ic, &pkt);
    if (re != 0) {
        canDecode = false;
        mMutex.unlock();
        return;
    }

    if (pkt.stream_index == videoStream) {
        //35ms
        re = avcodec_send_packet(videoCtx, &pkt);
        if (re != 0) {
            av_packet_unref(&pkt);
            mMutex.unlock();
            return;
        }
        re = avcodec_receive_frame(videoCtx, yuv);
        if (re != 0) {
            av_packet_unref(&pkt);
            mMutex.unlock();
            return;
        }
        swsCtx = sws_getCachedContext(swsCtx, videoCtx->width, videoCtx->height, videoCtx->pix_fmt,
                                      videoWidth, videoHeight, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL,
                                      NULL, NULL);
        if (!swsCtx) {
            av_packet_unref(&pkt);
            mMutex.unlock();
            return;
        }


    }

    av_packet_unref(&pkt);
    mMutex.unlock();

}

void FFmpegPlayer::Display(ANativeWindow *nativeWindow, ANativeWindow_Buffer *window_buffer) {
    mMutex.lock();
    if (!yuv || !swsCtx) {
        mMutex.unlock();
        return;
    }
    ANativeWindow_lock(nativeWindow, window_buffer, 0);
    sws_scale(swsCtx, (uint8_t const *const *) yuv->data, yuv->linesize, 0,
                      videoCtx->height, rgbFrame->data, rgbFrame->linesize);
    int dstStride = window_buffer->stride * 4;
    uint8_t *src = rgbFrame->data[0];
    int srcStride = rgbFrame->linesize[0];

    uint8_t *dst = (uint8_t *) window_buffer->bits;

    for (int row = 0; row < videoHeight; row++) {
        memcpy(dst + row * dstStride, src + row * srcStride, srcStride);
    }
    ANativeWindow_unlockAndPost(nativeWindow);
    mMutex.unlock();
}




void FFmpegPlayer::Close() {
    if (!yuv)
        av_frame_free(&yuv);

    if (!rgbFrame)
        av_frame_free(&yuv);

    if (!videoCtx)
        avcodec_close(videoCtx);

    if (!swsCtx)
        sws_freeContext(swsCtx);

    if (!ic)
        avformat_close_input(&ic);
}

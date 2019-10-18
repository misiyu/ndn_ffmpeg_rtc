//
// Created by mingj on 2019/10/18.
//

#include "EasyEncoder.h"
#include <iostream>

using namespace std;

EasyEncoder::EasyEncoder(enum AVCodecID codecId) {
    avcodec_register_all();
    pCodec = avcodec_find_encoder(codecId);
    if (!pCodec) {
        EasyEncoder::throwException("Could not find decoder");
    }
    pCodecContext = avcodec_alloc_context3(pCodec);
    if (!pCodecContext) {
        EasyEncoder::throwException("Could not allocate video codec context");
    }

    if (codecId == AV_CODEC_ID_H264) {
        av_opt_set(pCodecContext->priv_data, "preset", "slow", 0);
        av_opt_set(pCodecContext->priv_data, "tune", "zerolatency", 0);
    }
}


EasyEncoder::~EasyEncoder() {
    avcodec_close(pCodecContext);
    av_free(pCodecContext);
//    av_freep(&pFrame->data[0]);
//    av_frame_free(&pFrame);
}

EasyEncoder *EasyEncoder::initCodecParam(const CodecContextParam &param) {
    pCodecContext->bit_rate = param.bit_rate;
    pCodecContext->width = param.width;
    pCodecContext->height = param.height;
    pCodecContext->time_base = param.time_base;
    pCodecContext->gop_size = param.gop_size;
    pCodecContext->max_b_frames = param.max_b_frames;
    pCodecContext->pix_fmt = param.pix_fmt;
    return this;
}

EasyEncoder *EasyEncoder::prepareEncode() {
    // 打开编码器
    if (avcodec_open2(pCodecContext, pCodec, nullptr) < 0) {
        EasyEncoder::throwException("Could not open codec");
    } else {
        cout << "avcodec open success" << endl;
    }
//    pFrame = av_frame_alloc();
//    if (!pFrame) {
//        EasyEncoder::throwException("Could not allocate video frame");
//    }
//    pFrame->format = pCodecContext->pix_fmt;
//    pFrame->width = pCodecContext->width;
//    pFrame->height = pCodecContext->height;
//    int ret = av_image_alloc(pFrame->data, pFrame->linesize, pCodecContext->width, pCodecContext->height,
//                             pCodecContext->pix_fmt,
//                             16);
//    if (ret < 0) {
//        EasyEncoder::throwException("Could not allocate raw picture buffer");
//    }
    return this;
}

void EasyEncoder::throwException(const std::string &msg) {
    std::string logMsg = "EasyEncoder -> " + msg;
    cout << logMsg << endl;
    throw FFmpegFailedException(logMsg);
}

EasyEncoder *EasyEncoder::encode(AVFrame *pFrame, const EasyEncoder::EncodeCallbackFunc &callback) {
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;
    int ret = avcodec_send_frame(pCodecContext, pFrame);
    if (ret != 0) {
        std::cout << "fuck: " << ret << std::endl;
        return this;
    }
    while (avcodec_receive_packet(pCodecContext, &pkt) >= 0) {
        callback(&pkt);
    }
    av_packet_unref(&pkt);
    return this;
}

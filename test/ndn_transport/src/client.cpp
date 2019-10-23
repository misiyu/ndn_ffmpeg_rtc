//
// Created by mingj on 2019/10/22.
//

#include <iostream>
#define USE_FFMPEG
#include <SDL2Helper.h>
#include <EasyDecoder.h>
int main() {
    int screenW = 640;
    int screenH = 480;
    SDL2Helper sdl2Helper(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
    sdl2Helper.createWindow("My Camera Capture test Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            screenW, screenH)
            ->createRenderer();

    SDL_Texture *texture = sdl2Helper.createTexture(SDL_PIXELFORMAT_YV12,
                                                    SDL_TEXTUREACCESS_STREAMING, screenW,
                                                    screenH);
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = screenW;
    rect.h = screenH;
    EasyDecoder easyDecoder(AV_CODEC_ID_H264);
    easyDecoder.prepareDecode();
    SDL_Event e;
    bool exit = false;
    while(!exit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                exit = true;
            }
        }

        // 在这里接收数据，并得到Packet

        easyDecoder.decode(pkt, [=, &sdl2Helper](
                AVFrame *frame1) {
            sdl2Helper.updateYUVTexture(texture, &rect, frame1);
            sdl2Helper.renderClear()
                    ->renderCopy(texture, nullptr, &rect)
                    ->renderPresent();
        });
    }
}
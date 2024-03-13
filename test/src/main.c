#include <stdio.h>
#include "video_decoder.h"

void image_print(Frame video){
    for(int i=0;i<video.height;i+=6){
        for(int j=0;j<=video.width;j+=6){
            int index=(i*video.height+j)*3;
            int R=(int)video.data[index];
            int G=(int)video.data[index+1];
            int B=(int)video.data[index+2];
            printf("\33[38;2;%d;%d;%dm█\33[0m",R,G,B);
        }
        printf("\n");
    }

}

int main(){
    int a = decoder_init("./source/dragon.mp4");
    Frame cut=decoder_get_frame();
    image_print(cut);
    return 0;
}

//gcc src/main.c -I include/ -L lib/ -l videodecoder -l avformat -l avcodec -l avutil -l swscale -o main.out

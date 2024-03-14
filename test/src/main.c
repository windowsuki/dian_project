#include <stdio.h>
#include "video_decoder.h"
#include <stdlib.h>
#include <unistd.h>


void image_print(Frame cut, int s) { //average pooling
    for (int i = 0; i < cut.height; i += s) {
        for (int j = 0; j <= cut.width; j += s) {
            int index = i * cut.width + j;
            int R_sum = 0, G_sum = 0, B_sum = 0;
            for (int x = 0; x < s; x++) {
                for (int y = 0; y < s; y++) {
                    R_sum += cut.data[(index + x + y * cut.width) * 3];
                    G_sum += cut.data[(index + x + y * cut.width) * 3 + 1];
                    B_sum += cut.data[(index + x + y * cut.width) * 3 + 2];
                }
            }
            int R = R_sum / s ^ 2;
            int G = G_sum / s ^ 2;
            int B = B_sum / s ^ 2;
            printf("\33[38;2;%d;%d;%dm█", R, G, B);
        }
        printf("\n");
    }
}

void image_print_gray(Frame cut, int s) { //average pooling
    for (int i = 0; i < cut.height; i += s) {
        for (int j = 0; j <= cut.width; j += s) {
            int index = i * cut.width + j;
            int R_sum = 0, G_sum = 0, B_sum = 0;
            for (int x = 0; x < s; x++) {
                for (int y = 0; y < s; y++) {
                    R_sum += cut.data[(index + x + y * cut.width) * 3];
                    G_sum += cut.data[(index + x + y * cut.width) * 3 + 1];
                    B_sum += cut.data[(index + x + y * cut.width) * 3 + 2];
                }
            }
            int R = R_sum / s ^ 2;
            int G = G_sum / s ^ 2;
            int B = B_sum / s ^ 2;
            int Gray = (R + G + B) / 3;
            printf("\33[38;2;%d;%d;%dm█", Gray, Gray, Gray);
        }
        printf("\n");
    }
}


int main() {
    int a = decoder_init("./source/bad_apple.mp4");
    Frame cut;
    for (int i = 1; i < get_total_frames(); i++) {
        cut = decoder_get_frame();
        system("clear");
        image_print(cut, 6);
        sleep(0.1);
    }
    return 0;
}

//gcc src/main.c -I include/ -L lib/ -l videodecoder -l avformat -l avcodec -l avutil -l swscale -o main.out

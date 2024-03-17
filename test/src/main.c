#include <stdio.h>
#include "video_decoder.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

extern char* optarg;
int old_print[1000000];


void image_print_average(Frame cut, int s) { //average pooling  //两个average pooling还没改成优化后的模式
    for (int i = 0; i < cut.height; i += s) {
        for (int j = 0; j <= cut.width; j += s) {
            int R_sum = 0, G_sum = 0, B_sum = 0;
            for (int y = 0; y < s; y++) {
                for (int x = 0; x < s; x++) {
                    int index = (i + y) * cut.width + j + x;
                    R_sum += cut.data[index * 3];
                    G_sum += cut.data[index * 3 + 1];
                    B_sum += cut.data[index * 3 + 2];
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

void image_print_gray_average(Frame cut, int s) {
    for (int i = 0; i < cut.height; i += s) {
        for (int j = 0; j <= cut.width; j += s) {
            int index = i * cut.width + j;
            int R_sum = 0, G_sum = 0, B_sum = 0;
            for (int y = 0; y < s; y++) {
                for (int x = 0; x < s; x++) {
                    int index = (i + y) * cut.width + j + x;
                    R_sum += cut.data[index * 3];
                    G_sum += cut.data[index * 3 + 1];
                    B_sum += cut.data[index * 3 + 2];
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

void image_print_max_init(Frame cut, int s) { //max pooling
    printf("\033[1;1H");
    for (int i = 0; i < cut.height; i += s) {
        for (int j = 0; j <= cut.width; j += s) {
            int R_max = 0, G_max = 0, B_max = 0;
            int index0 = i * cut.width + j;
            for (int y = 0; y < s; y++) {
                for (int x = 0; x < s; x++) {
                    int index = (i + y) * cut.width + j + x;
                    R_max = (R_max > cut.data[index * 3]) ? R_max : cut.data[index * 3];
                    G_max = (G_max > cut.data[index * 3] + 1) ? G_max : cut.data[index * 3 + 1];
                    B_max = (B_max > cut.data[index * 3] + 1) ? B_max : cut.data[index * 3 + 2];
                }
            }
            printf("\033[38;2;%d;%d;%dm█", R_max, G_max, B_max);
            old_print[index0 * 3] = R_max;
            old_print[index0 * 3 + 1] = G_max;
            old_print[index0 * 3 + 2] = B_max;
        }
        printf("\n");
    }
}

void image_print_max(Frame cut, int s) { //max pooling
    printf("\033[1;1H");
    for (int i = 0; i < cut.height; i += s) {
        for (int j = 0; j < cut.width; j += s) {
            int R_max = 0, G_max = 0, B_max = 0;
            int index0 = i * cut.width + j;
            for (int y = 0; y < s; y++) {
                for (int x = 0; x < s; x++) {
                    int index = (i + y) * cut.width + j + x;
                    R_max = (R_max > cut.data[index * 3]) ? R_max : cut.data[index * 3];
                    G_max = (G_max > cut.data[index * 3] + 1) ? G_max : cut.data[index * 3 + 1];
                    B_max = (B_max > cut.data[index * 3] + 1) ? B_max : cut.data[index * 3 + 2];
                }
            }
            if (R_max != (old_print[index0 * 3]) & (j != (cut.width - 1)))
                printf("\033[38;2;%d;%d;%dm█", R_max, G_max, B_max);
            else
                printf("\033[1C");
            old_print[index0 * 3] = R_max;
            old_print[index0 * 3 + 1] = G_max;
            old_print[index0 * 3 + 2] = B_max;
        }
        printf("\n");
    }
}

void image_print_max_gray(Frame cut, int s) {
    printf("\033[1;1H");
    for (int i = 0; i < cut.height; i += s) {
        for (int j = 0; j <= cut.width; j += s) {
            int index0 = i * cut.width + j;
            int R_max = 0, G_max = 0, B_max = 0;
            for (int y = 0; y < s; y++) {
                for (int x = 0; x < s; x++) {
                    int index = (i + y) * cut.width + j + x;
                    R_max = (R_max > cut.data[index * 3]) ? R_max : cut.data[index * 3];
                    G_max = (G_max > cut.data[index * 3] + 1) ? G_max : cut.data[index * 3 + 1];
                    B_max = (B_max > cut.data[index * 3] + 1) ? B_max : cut.data[index * 3 + 2];
                }
            }
            int Gray = (R_max + G_max + B_max) / 3;
            if (R_max != (old_print[index0 * 3]) & (j != (cut.width - 1)))
                printf("\033[38;2;%d;%d;%dm█", Gray, Gray, Gray);
            else
                printf("\033[1C");
            old_print[index0 * 3] = R_max;
            old_print[index0 * 3 + 1] = G_max;
            old_print[index0 * 3 + 2] = B_max;
        }
        printf("\n");
    }
}

int main(int argc, char* argv[]) {
    int opt = 0, stride = 10, rgbmode = 0;
    char filename[] = "./source/bad_apple.mp4";
    while (opt = getopt(argc, argv, "hvcr:") != -1) {   //命令行解析
        switch (opt) {
        case 'h':
            printf("this is a help");
            return 0;
        case 'v':
            printf("dian_project v0.1");
            return 0;
        case 'c':
            rgbmode = 1;
            break;
        case 'r':
            stride = *optarg;
            break;
        case 'f':
            strcpy(filename, optarg);
            break;
        default:
            break;
        }
    }
    int a = decoder_init(filename);
    Frame cut;
    cut = decoder_get_frame();
    system("clear");
    image_print_max_init(cut, stride);  //先打印一遍，初始化old_print
    for (int i = 1; i < get_total_frames(); i++) {
        sleep(0.5);
        cut = decoder_get_frame();
        if (rgbmode) image_print_max(cut, stride);
        else image_print_max_gray(cut, stride);
    }
    return 0;
}

//gcc src/main.c -I include/ -L lib/ -l videodecoder -l avformat -l avcodec -l avutil -l swscale -o main.out

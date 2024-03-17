#include <stdio.h>
#include "video_decoder.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

extern char* optarg;

unsigned char b0[10000];
Frame buffer0 = { 0,0,0,b0 };


struct Param {
    void* buffer;
    int stride;
};

pthread_mutex_t mutex;
pthread_cond_t cond_resize, cond_print;


void* resize(void* arg) {
    struct Param* temp_argc;
    temp_argc = (struct Param*)arg;
    Frame cut;
    Frame* buffer = (Frame*)temp_argc->buffer;
    int stride = temp_argc->stride;
    for (int i = 1; i < get_total_frames(); i++) {
        Frame cut = decoder_get_frame();
        pthread_mutex_lock(&mutex);
        buffer->height = cut.height / stride;
        buffer->width = cut.width / stride;
        for (int i = 0; i < cut.height; i += stride) {
            for (int j = 0; j <= cut.width; j += stride) {
                int R_max = 0, G_max = 0, B_max = 0;
                int new_index = i * cut.width / stride + j / stride;
                for (int y = 0; y < stride; y++) {
                    for (int x = 0; x < stride; x++) {
                        int index = (i + y) * cut.width + j + x;
                        R_max = (R_max > cut.data[index * 3]) ? R_max : cut.data[index * 3];
                        G_max = (G_max > cut.data[index * 3] + 1) ? G_max : cut.data[index * 3 + 1];
                        B_max = (B_max > cut.data[index * 3] + 1) ? B_max : cut.data[index * 3 + 2];
                    }
                }
                buffer->data[new_index] = R_max;
                buffer->data[new_index + 1] = G_max;
                buffer->data[new_index + 2] = B_max;
            }
        }
        pthread_cond_signal(&cond_print);
        pthread_cond_wait(&cond_resize, &mutex);
        pthread_mutex_unlock(&mutex);
    }
    return ((void*)0);
}

void* image_print(void* arg) {
    Frame* buffer = (Frame*)arg;
    while (1) {
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < buffer->height; i++) {
            for (int j = 0; j <= buffer->width; j++) {
                int index = (i * buffer->width + j);
                int R = buffer->data[index];
                int G = buffer->data[index + 1];
                int B = buffer->data[index + 2];
                printf("\33[38;2;%d;%d;%dm█", R, G, B);
            }
            printf("\n");
        }
        pthread_cond_signal(&cond_resize);
        pthread_cond_wait(&cond_print, &mutex);
        pthread_mutex_unlock(&mutex);
    }
}

void image_print_average(Frame cut, int s) { //average pooling
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

void image_print_max(Frame cut, int s) { //max pooling
    for (int i = 0; i < cut.height; i += s) {
        for (int j = 0; j <= cut.width; j += s) {
            int R_max = 0, G_max = 0, B_max = 0;
            for (int y = 0; y < s; y++) {
                for (int x = 0; x < s; x++) {
                    int index = (i + y) * cut.width + j + x;
                    R_max = (R_max > cut.data[index * 3]) ? R_max : cut.data[index * 3];
                    G_max = (G_max > cut.data[index * 3] + 1) ? G_max : cut.data[index * 3 + 1];
                    B_max = (B_max > cut.data[index * 3] + 1) ? B_max : cut.data[index * 3 + 2];
                }
            }
            printf("\33[38;2;%d;%d;%dm█", R_max, G_max, B_max);
        }
        printf("\n");
    }
}

void image_print_max_gray(Frame cut, int s) {
    for (int i = 0; i < cut.height; i += s) {
        for (int j = 0; j <= cut.width; j += s) {
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
            printf("\33[38;2;%d;%d;%dm█", Gray, Gray, Gray);
        }
        printf("\n");
    }
}


int main(int argc, char* argv[]) {  //segmentation fault寄了
    int opt = 0, stride = 6, rgbmode = 0;
    char filename[] = "./source/bad_apple.mp4";
    // while(opt=getopt(argc,argv,"hvcr:")!=-1){
    //     switch (opt){
    //     case 'h':
    //         printf("this is a help");
    //         return 0;
    //     case 'v':
    //         printf("dian_project v0.2");
    //         return 0;
    //     case 'c':
    //         rgbmode=1;
    //         break;
    //     case 'r':
    //         stride=*optarg;
    //         break;
    //     case 'f':
    //         strcpy(filename,optarg);
    //         break;
    //     default:
    //         break;
    //     }
    // }
    int a = decoder_init(filename);
    unsigned char b[10000];
    Frame cut;
    Frame buffer = { 0,0,0,b };
    pthread_t pth_print, pth_resize;
    cut = decoder_get_frame();
    struct Param temp_argc;
    temp_argc.buffer = &buffer;
    temp_argc.stride = stride;
    pthread_create(&pth_resize, NULL, resize, (void*)&temp_argc);
    pthread_create(&pth_print, NULL, image_print, (void*)&buffer);
    pthread_join(pth_resize, NULL);
    pthread_join(pth_print, NULL);
    pthread_mutex_destroy(&mutex);
    while (1);
    return 0;
}

//gcc src/main.c -I include/ -L lib/ -l videodecoder -l avformat -l avcodec -l avutil -l swscale -o main.out

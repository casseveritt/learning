#pragma once

unsigned char* image_load(const char* filename, int* w, int* h, int* n);
void image_free(unsigned char* img);
void image_store_png(const char* filename, int w, int h, int n, unsigned char* img);

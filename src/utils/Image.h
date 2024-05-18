#ifndef APEP_APP_IMAGE_H
#define APEP_APP_IMAGE_H

#include <vector>

#include "imgui.h"

// Struct to store 2D vectors as 2D arrays
struct Image {
    int nx, ny;
    float *data;
    float aspect_ratio;

    Image(int nx, int ny, std::vector<std::vector<float>> value);
    ~Image();

    float *GetImage();
    ImVec2 GetWindowSize();

};

Image::Image(int nx, int ny, std::vector<std::vector<float>> value) {
    // Important: We transpose the data here to match the ImPlot heatmap
    this->nx = ny;
    this->ny = nx;
    this->data = new float[ny * nx];
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            data[j * nx + i] = value[i][ny-(j+1)];
        }
    }
}

Image::~Image() {
    delete[] data;
}

float *Image::GetImage() {
    // Returns a pointer to the first element of the data array
    return data;
}

ImVec2 Image::GetWindowSize() {
    // Returns the window size
    int width = 640;
    float aspect_ratio = (float) nx / ny;
    int height = static_cast<int>(width * aspect_ratio);

    return ImVec2(width, height);
}
#endif //APEP_APP_IMAGE_H

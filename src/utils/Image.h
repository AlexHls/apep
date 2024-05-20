#ifndef APEP_APP_IMAGE_H
#define APEP_APP_IMAGE_H

#include <iostream>
#include <ostream>
#include <vector>

#include "imgui.h"

// Struct to store 2D vectors as 2D arrays
struct Image {
    int nx, ny;
    float *data;
    float aspect_ratio;

    Image(int nghost, int nx, int ny, const std::vector<std::vector<float> > &value);

    void Print();

    ~Image();

    float *GetImage();

    ImVec2 GetWindowSize() const;
};

inline Image::Image(const int nghost, const int nx, const int ny,
                    const std::vector<std::vector<float> > &value) {
    // Important: We transpose the data here to match the ImPlot heatmap
    this->nx = ny;
    this->ny = nx;
    this->data = new float[ny * nx];
    const int max = ny + nghost;
    aspect_ratio = static_cast<float>(nx) / ny;
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            data[j * nx + i] = value[nghost + i][max - (j + 1)];
        }
    }
}

void Image::Print() {
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            std::cout << data[i * ny + j] << " ";
        }
        std::cout << std::endl;
    }
}

inline Image::~Image() {
    delete[] data;
}

float *Image::GetImage() {
    // Returns a pointer to the first element of the data array
    return data;
}

inline ImVec2 Image::GetWindowSize() const {
    // Returns the window size
    constexpr float height = 640.0f;
    const float width = height * aspect_ratio;

    return ImVec2(width, height);
}
#endif //APEP_APP_IMAGE_H

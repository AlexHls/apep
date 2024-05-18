#ifndef APEP_HYDRO_GRID_H
#define APEP_HYDRO_GRID_H
#include <vector>

struct Grid {
    std::vector<std::vector<float>> rho;
    int nx, ny, boundary_y;
    float rho_ini_lower, rho_ini_upper;

    ~Grid() = default;

    void Update();

    // Functions for RT Instability
    Grid(const struct RTSettings &settings);
    void Reset(const struct RTSettings &settings);
};

#endif //APEP_HYDRO_GRID_H

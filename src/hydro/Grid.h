#ifndef APEP_HYDRO_GRID_H
#define APEP_HYDRO_GRID_H
#include <vector>

struct Grid {
    std::vector<std::vector<float>> rho;
    std::vector<std::vector<float>> en;
    std::vector<std::vector<float>> u;
    std::vector<std::vector<float>> v;
    std::vector<std::vector<float>> gx;
    std::vector<std::vector<float>> gy;
    int nx, ny, nghost;
    float rho_ini_lower, rho_ini_upper;
    float en_ini;
    float grav_x_ini, grav_y_ini;
    float perturb_strength;
    float x1, x2, y1, y2, dlx, dly;
    float time;

    ~Grid() = default;

    void Update();
    void Resize();
    void WriteGrid();

    // Functions for RT Instability
    Grid(const struct RTSettings &settings);
    void Reset(const struct RTSettings &settings);

    void AttrsFromSettings(const struct RTSettings &settings);
    void RTInstability();

};

#endif //APEP_HYDRO_GRID_H

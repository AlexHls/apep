#ifndef APEP_HYDRO_GRID_H
#define APEP_HYDRO_GRID_H
#include <vector>

#include "Hydro.h"
#include "Reconstruct.h"
#include "RiemannSolver.h"

struct Grid {
    std::vector<std::vector<float> > rho;
    std::vector<std::vector<float> > en;
    std::vector<std::vector<float> > u;
    std::vector<std::vector<float> > v;
    std::vector<std::vector<float> > gx;
    std::vector<std::vector<float> > gy;
    QVec2 cons;
    int nx, ny, nghost;
    int nxg, nyg; // nxg = nx + 2 * nghost, nyg = ny + 2 * nghost
    int nxmg, nymg; // nxmg = nxg - nghost, nymg = nyg - nghost
    float rho_ini_lower, rho_ini_upper;
    float en_ini;
    float grav_x_ini, grav_y_ini;
    float perturb_strength;
    float x1, x2, y1, y2, dlx, dly;
    float time, dt;
    float cfl;
    float gamma_ad;
    int reconstruct_type;
    int riemann_solver_type;
    Reconstructor *reconstructor;
    RiemannSolver *riemann_solver;

    ~Grid() = default;

    void Clear();

    void Update();

    void Resize();

    void WriteGrid();

    // Functions for converting between conserved and primitive variables
    void PrimToCons();

    void ConsToPrim();

    // Hydrodynamics functions
    void TimeStep();

    void ApplyBoundaryConditions();

    // Functions for RT Instability
    Grid(struct RTSettings &settings);

    void Reset(struct RTSettings &settings);

    void AttrsFromSettings(struct RTSettings &settings);

    void RTInstability();
};

#endif //APEP_HYDRO_GRID_H

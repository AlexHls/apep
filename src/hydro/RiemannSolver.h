#ifndef APEP_HYDRO_RIEMANN_H
#define APEP_HYDRO_RIEMANN_H

#include "Hydro.h"

enum RiemannSolverType {
    HLLE = 0,
    HLLC = 1,
};

struct RiemannSolver {
    const int nx, ny, nghost;
    const int rs;

    RiemannSolver(int nx, int ny, int nghost, int rs);

    RiemannSolver();

    ~RiemannSolver() = default;

    void Solve(const struct QVec &ql, const struct QVec &qr, struct QVec &flux, const float gamma_ad);

    void SolveHLLC(const struct QVec &ql, const struct QVec &qr, struct QVec &flux, const float gamma_ad);

    void SolveHLLE(const struct QVec &ql, const struct QVec &qr, struct QVec &flux, const float gamma_ad);
};

#endif //APEP_HYDRO_RIEMANN_H

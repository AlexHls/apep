#include "RiemannSolver.h"

#include <cmath>
#include <iostream>

#include "Reconstruct.h"

int sign(const float val) {
    return std::signbit(val) ? -1 : 1;
}

RiemannSolver::RiemannSolver(int nx, int ny, int nghost, int rs) : nx(nx), ny(ny), nghost(nghost), rs(rs) {
}

void RiemannSolver::Solve(const struct QVec &ql, const struct QVec &qr, struct QVec &flux, const float gamma_ad,
                          const int dir) {
    if (rs == HLLE) {
        SolveHLLE(ql, qr, flux, gamma_ad, dir);
    } else if (rs == HLLC) {
        SolveHLLC(ql, qr, flux, gamma_ad, dir);
    }
}

void RiemannSolver::SolveHLLC(const struct QVec &ql, const struct QVec &qr, struct QVec &flux, const float gamma_ad,
                              const int dir) {
    // Solve HLLC
    const int idx_max = dir == XDIR ? nx + 1 : ny + 1;
    for (int i = 0; i < idx_max; i++) {
        // Left states
        const float rhol = ql.rho[i];
        const float ul = ql.u[i];
        const float vl = ql.v[i];
        const float pl = ql.en[i];
        const float vel2l = ul * ul + vl * vl;
        const float el = pl / (gamma_ad - 1) + 0.5f * rhol * vel2l;

        // Right states
        const float rhor = qr.rho[i];
        const float ur = qr.u[i];
        const float vr = qr.v[i];
        const float pr = qr.en[i];
        const float vel2r = ur * ur + vr * vr;
        const float er = pr / (gamma_ad - 1) + 0.5f * rhor * vel2r;

        const float cl = gamma_ad * pl / rhol;
        const float cr = gamma_ad * pr / rhor;

        const float cmax = std::sqrt(std::max(cl, cr));

        const float sl = std::min(ul, ur) - cmax;
        const float sr = std::max(ul, ur) + cmax;

        const float dsul = sl - ul;
        const float dsur = sr - ur;

        const float ustar = (pr - pl + rhol * ul * dsul - rhor * ur * dsur) / (rhol * dsul - rhor * dsur);

        const float rhobar = 0.5 * (rhol + rhor);
        const float cbar = std::sqrt(0.5 * (cl + cr));

        const float pstar = 0.5 * (pl + pr) - 0.5 * rhobar * cbar * (ur - ul);

        const int sgn = sign(ustar);

        const float rhostarl = rhol * (dsul / (sl - ustar));
        const float estarl = rhostarl * (el / rhol + (ustar - ul) * (ustar + pl / rhol / dsul));

        const float rhostarr = rhor * (dsur / (sr - ustar));
        const float estarr = rhostarr * (er / rhor + (ustar - ur) * (ustar + pr / rhor / dsur));

        const float onemsignh = 0.5 * (1.0 - sgn);
        const float onepsignh = 0.5 * (1.0 + sgn);

        const float rhostar = onepsignh * rhostarl + onemsignh * rhostarr;
        const float rhoustar = rhostar * ustar;

        // Calculate fluxes
        flux.rho[i] = rhoustar;
        flux.u[i] = rhoustar * ustar + pstar;
        flux.v[i] = rhoustar * (onepsignh * vl + onemsignh * vr);
        flux.en[i] = (onepsignh * estarl + onemsignh * estarr + pstar) * ustar;
    }
}

void RiemannSolver::SolveHLLE(const struct QVec &ql, const struct QVec &qr, struct QVec &flux, const float gamma_ad,
                              const int dir) {
    // Solve HLLE
}

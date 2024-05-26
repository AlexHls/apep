#include "Reconstruct.h"

#include <cmath>

static float minmod(const float &a, const float &b) {
    if (a * b <= 0.0f) return 0.0;
    return (std::abs(a) < std::abs(b)) ? a : b;
}

static float get_slope(const float &ql, const float &q, const float &qr) {
    const float dql = q - ql;
    const float dqr = qr - q;
    return minmod(dql, dqr);
}

Reconstructor::Reconstructor(const int nx, const int ny, const int nghost, const int rct) : nx(nx), ny(ny),
    nghost(nghost), rct(rct) {
}

void Reconstructor::Reconstruct(const struct QVec &q, struct QVec &ql, struct QVec &qr, const int dir) {
    if (rct == CONSTANT) {
        ReconstructConstant(q, ql, qr, dir);
    } else if (rct == LINEAR) {
        ReconstructLinear(q, ql, qr, dir);
    }
}

void Reconstructor::ReconstructConstant(const struct QVec &q, struct QVec &ql,
                                        struct QVec &qr, const int dir) {
    // Reconstruct constant in x-direction
    if (dir == XDIR) {
        for (int i = 0; i < nx + 1; i++) {
            ql.Set(i, q.rho[i], q.u[i], q.v[i], q.en[i]);
            qr.Set(i, q.rho[i + 1], q.u[i + 1], q.v[i + 1], q.en[i + 1]);
        }
    } else if (dir == YDIR) {
        // Reconstruct constant in y-direction
        for (int j = 0; j < ny + 1; j++) {
            ql.Set(j, q.rho[j], q.u[j], q.v[j], q.en[j]);
            qr.Set(j, q.rho[j + 1], q.u[j + 1], q.v[j + 1], q.en[j + 1]);
        }
    }
}

void Reconstructor::ReconstructLinear(const struct QVec &q, struct QVec &ql,
                                      struct QVec &qr, const int dir) {
    // Reconstruct linear
    if (dir == XDIR) {
        for (int i = 0; i < nx + 1; i++) {
            // Left side
            ql.rho[i] = q.rho[nghost + i - 1] + 0.5f * get_slope(q.rho[nghost + i - 2], q.rho[nghost + i - 1],
                                                                 q.rho[nghost + i]);
            ql.u[i] = q.u[nghost + i - 1] + 0.5f * get_slope(q.u[nghost + i - 2], q.u[nghost + i - 1], q.u[nghost + i]);
            ql.v[i] = q.v[nghost + i - 1] + 0.5f * get_slope(q.v[nghost + i - 2], q.v[nghost + i - 1], q.v[nghost + i]);
            ql.en[i] = q.en[nghost + i - 1] + 0.5f *
                       get_slope(q.en[nghost + i - 2], q.en[nghost + i - 1], q.en[nghost + i]);

            // Right side
            qr.rho[i] = q.rho[nghost + i] - 0.5f * get_slope(q.rho[nghost + i - 1], q.rho[nghost + i],
                                                             q.rho[nghost + i + 1]);
            qr.u[i] = q.u[nghost + i] - 0.5f * get_slope(q.u[nghost + i - 1], q.u[nghost + i], q.u[nghost + i + 1]);
            qr.v[i] = q.v[nghost + i] - 0.5f * get_slope(q.v[nghost + i - 1], q.v[nghost + i], q.v[nghost + i + 1]);
            qr.en[i] = q.en[nghost + i] - 0.5f *
                       get_slope(q.en[nghost + i - 1], q.en[nghost + i], q.en[nghost + i + 1]);
        }
    } else if (dir == YDIR) {
        for (int j = 0; j < ny + 1; j++) {
            // Left side
            ql.rho[j] = q.rho[nghost + j - 1] + 0.5f * get_slope(q.rho[nghost + j - 2], q.rho[nghost + j - 1],
                                                                 q.rho[nghost + j]);
            ql.u[j] = q.u[nghost + j - 1] + 0.5f * get_slope(q.u[nghost + j - 2], q.u[nghost + j - 1], q.u[nghost + j]);
            ql.v[j] = q.v[nghost + j - 1] + 0.5f * get_slope(q.v[nghost + j - 2], q.v[nghost + j - 1], q.v[nghost + j]);
            ql.en[j] = q.en[nghost + j - 1] + 0.5f *
                       get_slope(q.en[nghost + j - 2], q.en[nghost + j - 1], q.en[nghost + j]);

            // Right side
            qr.rho[j] = q.rho[nghost + j] - 0.5f * get_slope(q.rho[nghost + j - 1], q.rho[nghost + j],
                                                             q.rho[nghost + j + 1]);
            qr.u[j] = q.u[nghost + j] - 0.5f * get_slope(q.u[nghost + j - 1], q.u[nghost + j], q.u[nghost + j + 1]);
            qr.v[j] = q.v[nghost + j] - 0.5f * get_slope(q.v[nghost + j - 1], q.v[nghost + j], q.v[nghost + j + 1]);
            qr.en[j] = q.en[nghost + j] - 0.5f *
                       get_slope(q.en[nghost + j - 1], q.en[nghost + j], q.en[nghost + j + 1]);
        }
    }
}

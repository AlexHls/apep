#include "Reconstruct.h"

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
}

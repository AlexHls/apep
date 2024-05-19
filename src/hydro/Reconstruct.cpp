#include "Reconstruct.h"

Reconstructor::Reconstructor(const int nx, const int ny, const int nghost, const int rct) : nx(nx), ny(ny),
    nghost(nghost), rct(rct) {
}

void Reconstructor::Reconstruct(const struct QVec &q, struct QVec &ql, struct QVec &qr) {
    if (rct == CONSTANT) {
        ReconstructConstant(q, ql, qr);
    } else if (rct == LINEAR) {
        ReconstructLinear(q, ql, qr);
    }
}

void Reconstructor::ReconstructConstant(const struct QVec &q, struct QVec &ql,
                                        struct QVec &qr) {
    // Reconstruct constant in x-direction
    for (int i = 0; i < nx + 1; i++) {
        if (i < nx + 1) {
            ql.Set(i, q.rho[i], q.u[i], q.v[i], q.en[i]);
        }
        if (i > 0) {
            qr.Set(i + 1, q.rho[i], q.u[i], q.v[i], q.en[i]);
        }
    }
}

void Reconstructor::ReconstructLinear(const struct QVec &q, struct QVec &ql,
                                      struct QVec &qr) {
    // Reconstruct linear
}

#ifndef APEP_HYDRO_RECONSTRUCT_H
#define APEP_HYDRO_RECONSTRUCT_H

#include "Hydro.h"

enum ReconstructType {
    CONSTANT = 0,
    LINEAR = 1,
};

enum ReconstructDirection {
    XDIR = 0,
    YDIR = 1,
};

struct Reconstructor {
    const int nx, ny, nghost;
    const int rct;

    Reconstructor(int nx, int ny, int nghost, int rct);

    Reconstructor();

    ~Reconstructor() = default;

    void Reconstruct(const struct QVec &q, struct QVec &ql, struct QVec &qr, const int dir);

    void ReconstructConstant(const struct QVec &q, struct QVec &ql, struct QVec &qr, const int dir);

    void ReconstructLinear(const struct QVec &q, struct QVec &ql, struct QVec &qr, const int dir);
};


#endif //APEP_HYDRO_RECONSTRUCT_H

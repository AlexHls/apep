#ifndef APEP_HYDRO_HYDRO_H
#define APEP_HYDRO_HYDRO_H

#include <vector>

struct QVec2 {
    std::vector<std::vector<float> > rho;
    std::vector<std::vector<float> > u;
    std::vector<std::vector<float> > v;
    std::vector<std::vector<float> > en;

    ~QVec2() = default;

    QVec2() = default;

    QVec2(const int nx, const int ny) {
        Resize(nx, ny);
    }

    void Resize(const int nx, const int ny) {
        rho.resize(nx);
        u.resize(nx);
        v.resize(nx);
        en.resize(nx);
        for (int i = 0; i < nx; i++) {
            rho[i].resize(ny);
            u[i].resize(ny);
            v[i].resize(ny);
            en[i].resize(ny);
        }
    }
};

struct QVec {
    std::vector<float> rho;
    std::vector<float> u;
    std::vector<float> v;
    std::vector<float> en;

    QVec(const size_t n) {
        rho.resize(n);
        u.resize(n);
        v.resize(n);
        en.resize(n);
    }

    void Set(const int i, const float rho, const float u, const float v, const float en) {
        this->rho[i] = rho;
        this->u[i] = u;
        this->v[i] = v;
        this->en[i] = en;
    }

    void Set(const std::vector<float> &rho, const std::vector<float> &u, const std::vector<float> &v,
             const std::vector<float> &en) {
        this->rho = rho;
        this->u = u;
        this->v = v;
        this->en = en;
    }

    void FlipVelocities() {
        for (size_t i = 0; i < u.size(); i++) {
            const float u_tmp = u[i];
            const float v_tmp = v[i];
            u[i] = v_tmp;
            v[i] = -u_tmp;
        }
    }
};

#endif //APEP_HYDRO_HYDRO_H

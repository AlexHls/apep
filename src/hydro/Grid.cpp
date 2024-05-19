#include "Grid.h"
#include "Reconstruct.h"

#include <cstdio>
#include <string>
#include <valarray>

#include "Image.h"
#include "Settings.h"

#include "imgui.h"
#include "implot.h"

void Grid::Update() {
    Image image(nghost, nx, ny, rho);
    auto image_size = image.GetWindowSize();

    static float scale_min = 0;
    static float scale_max = 3.0f;

    static ImPlotColormap map = ImPlotColormap_Viridis;
    if (ImPlot::ColormapButton(ImPlot::GetColormapName(map), ImVec2(225, 0), map)) {
        map = (map + 1) % ImPlot::GetColormapCount();
        // We bust the color cache of our plots so that item colors will
        // resample the new colormap in the event that they have already
        // been created. See documentation in implot.h.
        ImPlot::BustColorCache("##Heatmap1");
        ImPlot::BustColorCache("##Heatmap2");
    }

    ImGui::SameLine();
    ImGui::LabelText("##Colormap Index", "%s", "Change Colormap");
    ImGui::SetNextItemWidth(225);
    ImGui::DragFloatRange2("Min / Max", &scale_min, &scale_max, 0.01f, -20, 20);
    ImGui::Text("Current Time: %.3f", time);
    ImGui::Text("Current dlx: %.3f", dlx);
    ImGui::Text("Current dly: %.3f", dly);
    if (reconstruct_type == ReconstructType::CONSTANT) {
        ImGui::Text("Reconstruction: Constant");
    } else {
        ImGui::Text("Reconstruction: Linear");
    }

    // Display image size
    ImGui::Text("Image Size: %.0f x %.0f", image_size.x, image_size.y);
    if (ImGui::Button("Print Grid")) {
        image.Print();
    }

    if (ImGui::Button("Save Grid")) {
        WriteGrid();
    }

    static ImPlotAxisFlags axes_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines |
                                        ImPlotAxisFlags_NoTickMarks;

    ImPlot::PushColormap(map);

    if (ImPlot::BeginPlot("##Heatmap1", image_size, ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(nullptr, nullptr, axes_flags, axes_flags);
        ImPlot::PlotHeatmap("heat", image.GetImage(), image.nx, image.ny, scale_min, scale_max, nullptr,
                            ImPlotPoint(0, 0), ImPlotPoint(1, 1));
        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ColormapScale("##HeatScale", scale_min, scale_max, ImVec2(60, image_size.y));
}

Grid::Grid(const struct RTSettings &settings) {
    Reset(settings);
}

void Grid::Reset(const RTSettings &settings) {
    AttrsFromSettings(settings);
    Resize();
    RTInstability();
    PrimToCons();
}

void Grid::Clear() {
    delete reconstructor;
    delete riemann_solver;
}

void Grid::AttrsFromSettings(const struct RTSettings &settings) {
    this->nx = settings.nx;
    this->ny = settings.ny;
    this->nghost = settings.nghost;
    this->nxg = nx + 2 * nghost;
    this->nyg = ny + 2 * nghost;
    this->nxmg = nxg - nghost;
    this->nymg = nyg - nghost;
    this->rho_ini_lower = settings.rho_ini_lower;
    this->rho_ini_upper = settings.rho_ini_upper;
    this->en_ini = settings.en_ini;
    this->grav_x_ini = settings.grav_x_ini;
    this->grav_y_ini = settings.grav_y_ini;
    this->perturb_strength = settings.perturb_strength;
    this->x1 = settings.x1;
    this->x2 = settings.x2;
    this->y1 = settings.y1;
    this->y2 = settings.y2;
    this->dlx = (settings.x2 - settings.x1) / settings.nx;
    this->dly = (settings.y2 - settings.y1) / settings.ny;
    this->time = 0.0f;
    this->cfl = settings.cfl;
    this->dt = 0.5 * cfl * std::min(dlx, dly) / 3.5;
    this->gamma_ad = settings.gamma_ad;
    this->reconstruct_type = settings.reconstruct_type;
    this->riemann_solver_type = settings.riemann_solver_type;
    // Delete the old reconstructor and create a new one
    this->reconstructor = new Reconstructor(nx, ny, nghost, reconstruct_type);
    this->riemann_solver = new RiemannSolver(nx, ny, nghost, riemann_solver_type);
}

void Grid::Resize() {
    rho.resize(nxg);
    en.resize(nxg);
    u.resize(nxg);
    v.resize(nxg);
    gx.resize(nxg);
    gy.resize(nxg);
    for (int i = 0; i < nxg; i++) {
        rho[i].resize(nyg);
        en[i].resize(nyg);
        u[i].resize(nyg);
        v[i].resize(nyg);
        gx[i].resize(nyg);
        gy[i].resize(nyg);
    }
    cons.Resize(nx, ny);
}

void Grid::RTInstability() {
    // Initial condition setup for RT Instability
    for (int i = 0; i < nxg; i++) {
        for (int j = 0; j < nyg; j++) {
            const float xi = x1 + dlx * (i - 0.5f);
            const float yj = y1 + dly * (j - 0.5f);

            gx[i][j] = grav_x_ini;
            gy[i][j] = grav_y_ini;
            if (yj < 0.0f) {
                rho[i][j] = rho_ini_lower;
            } else {
                rho[i][j] = rho_ini_upper;
            }
            en[i][j] = en_ini + gy[i][j] * yj * rho[i][j];
            u[i][j] = 0.0f;
            v[i][j] = perturb_strength * (1.0f + std::cos(4.0f * M_PI * xi)) * (1.0f + std::cos(3.0f * M_PI * yj)) /
                      4.0f;
        }
    }
}

void Grid::WriteGrid() {
    const std::string filename = "grid.txt";

    FILE *file = fopen(filename.c_str(), "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename.c_str());
    }
    // Write a line containing the variable name, followed by the values
    fprintf(file, "# Density\n");
    for (int i = nghost; i < nxmg; i++) {
        for (int j = nghost; j < nymg; j++) {
            fprintf(file, "%f ", rho[i][j]);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "# Energy\n");
    for (int i = nghost; i < nxmg; i++) {
        for (int j = nghost; j < nymg; j++) {
            fprintf(file, "%f ", en[i][j]);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "# Velocity x\n");
    for (int i = nghost; i < nxmg; i++) {
        for (int j = nghost; j < nymg; j++) {
            fprintf(file, "%f ", u[i][j]);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "# Velocity y\n");
    for (int i = nghost; i < nxmg; i++) {
        for (int j = nghost; j < nymg; j++) {
            fprintf(file, "%f ", v[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void Grid::PrimToCons() {
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            cons.rho[i][j] = rho[i + nghost][j + nghost];
            cons.u[i][j] = rho[i + nghost][j + nghost] * u[i + nghost][j + nghost];
            cons.v[i][j] = rho[i + nghost][j + nghost] * v[i + nghost][j + nghost];
            cons.en[i][j] = en[i + nghost][j + nghost] * (1 / (gamma_ad - 1))
                            + 0.5f * rho[i + nghost][j + nghost] * (std::pow(u[i + nghost][j + nghost], 2) + std::pow(
                                                                        v[i + nghost][j + nghost], 2));
        }
    }
}

void Grid::ConsToPrim() {
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            static float rho_new = cons.rho[i][j];
            u[i + nghost][j + nghost] = cons.u[i][j] / rho_new;
            v[i + nghost][j + nghost] = cons.v[i][j] / rho_new;
            en[i + nghost][j + nghost] =
                    gamma_ad * (cons.en[i][j] - 0.5 / rho_new * (
                                    std::pow(cons.u[i][j], 2) + std::pow(cons.v[i][j], 2)));
            rho[i + nghost][j + nghost] = rho_new;
        }
    }
}

void Grid::TimeStep() {
    // Advance one time step

    // First, apply boundary conditions
    ApplyBoundaryConditions();

    QVec2 res(nx, ny);

    // Calculate the fluxes in x direction
    for (int j = 0; j < ny; j++) {
        QVec qlx(nx + 1), qrx(nx + 1), q(nxg);
        QVec fluxx(nx + 1);
        for (int i = 0; i < nx; i++) {
            q.Set(i, rho[i][j], u[i][j], v[i][j], en[i][j]);
        }
        reconstructor->Reconstruct(q, qlx, qrx);
        riemann_solver->Solve(qlx, qrx, fluxx, gamma_ad);
        for (int i = 0; i < nx; i++) {
            res.rho[i][j] = (fluxx.rho[i + 1] - fluxx.rho[i]) / dlx;
            res.u[i][j] = (fluxx.u[i + 1] - fluxx.u[i]) / dlx;
            res.v[i][j] = (fluxx.v[i + 1] - fluxx.v[i]) / dlx;
            res.en[i][j] = (fluxx.en[i + 1] - fluxx.en[i]) / dlx;
        }
    }

    // Calculate the fluxes in y direction
    for (int i = 0; i < nx; i++) {
        QVec qly(ny + 1), qry(ny + 1), q(nyg);
        QVec fluxy(ny + 1);
        for (int j = 0; j < ny; j++) {
            q.Set(j, rho[i][j], u[i][j], v[i][j], en[i][j]);
        }
        reconstructor->Reconstruct(q, qly, qry);
        riemann_solver->Solve(qly, qry, fluxy, gamma_ad);
        for (int j = 0; j < ny; j++) {
            res.rho[i][j] = (fluxy.rho[j + 1] - fluxy.rho[j]) / dly;
            res.u[i][j] = (fluxy.u[j + 1] - fluxy.u[j]) / dly;
            res.v[i][j] = (fluxy.v[j + 1] - fluxy.v[j]) / dly;
            res.en[i][j] = (fluxy.en[j + 1] - fluxy.en[j]) / dly;
        }
    }

    // Gravity update
    // TODO: Move this to it's own function
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            res.u[i][j] -= gx[i + nghost][j + nghost] * rho[i + nghost][j + nghost];
            res.v[i][j] -= gy[i + nghost][j + nghost] * rho[i + nghost][j + nghost];
            res.en[i][j] -= (gx[i + nghost][j + nghost] * u[i + nghost][j + nghost] +
                             gy[i + nghost][j + nghost] * v[i + nghost][j + nghost]) * rho[i + nghost][j + nghost];
        }
    }

    // Integrate result
    // TODO: Move this to it's own function
    QVec2 k2(nx, ny);
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            cons.rho[i][j] = cons.rho[i][j] + dt * res.rho[i][j];
            cons.u[i][j] = cons.u[i][j] + dt * res.u[i][j];
            cons.v[i][j] = cons.v[i][j] + dt * res.v[i][j];
            cons.en[i][j] = cons.en[i][j] + dt * res.en[i][j];
        }
    }

    ConsToPrim();
}

void Grid::ApplyBoundaryConditions() {
    // Apply boundary conditions
    // Periodic in x, reflecting in y
    for (int i = 0; i < nxg; i++) {
        for (int j = 0; j < nghost; j++) {
            rho[i][j] = rho[i][nyg - 2 * nghost + j];
            rho[i][nyg - nghost + j] = rho[i][nghost + j];
            en[i][j] = en[i][nyg - 2 * nghost + j];
            en[i][nyg - nghost + j] = en[i][nghost + j];
            u[i][j] = u[i][nyg - 2 * nghost + j];
            u[i][nyg - nghost + j] = u[i][nghost + j];
            v[i][j] = -v[i][nyg - 2 * nghost + j];
            v[i][nyg - nghost + j] = -v[i][nghost + j];
        }
    }
    for (int j = 0; j < nyg; j++) {
        for (int i = 0; i < nghost; i++) {
            rho[i][j] = rho[nxg - 2 * nghost + i][j];
            rho[nxg - nghost + i][j] = rho[nghost + i][j];
            en[i][j] = en[nxg - 2 * nghost + i][j];
            en[nxg - nghost + i][j] = en[nghost + i][j];
            u[i][j] = -u[nxg - 2 * nghost + i][j];
            u[nxg - nghost + i][j] = -u[nghost + i][j];
            v[i][j] = v[nxg - 2 * nghost + i][j];
            v[nxg - nghost + i][j] = v[nghost + i][j];
        }
    }
}


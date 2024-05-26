#include "Grid.h"
#include "Reconstruct.h"

#include <cstdio>
#include <string>
#include <valarray>

#include "Image.h"
#include "Settings.h"

#include "imgui.h"
#include "implot.h"

static constexpr float ALPHA[2][3] = {
    {1.0f, 0.0f, 1.0f},
    {0.5f, 0.5f, 0.5f}
};

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
    ImGui::Text(("Current dt: %.3f"), dt);
    ImGui::Text("Current dlx: %.3f", dlx);
    ImGui::Text("Current dly: %.3f", dly);
    if (reconstruct_type == ReconstructType::CONSTANT) {
        ImGui::Text("Reconstruction: Constant");
    } else {
        ImGui::Text("Reconstruction: Linear");
    }

    // Display image size
    ImGui::Text("Image Size: %.0f x %.0f", image_size.x, image_size.y);
    ImGui::Text("Image Grid Size: %.0f x %.0f", image.nx, image.ny);
    if (ImGui::Button("Print Grid")) {
        image.Print();
    }

    if (ImGui::Button("Save Grid")) {
        WriteGrid();
    }

    static ImPlotAxisFlags axes_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines |
                                        ImPlotAxisFlags_NoTickMarks;

    ImPlot::PushColormap(map);

    if (ImGui::BeginTabBar("Images", ImGuiTabBarFlags_None)) {
        if (ImGui::BeginTabItem("rho")) {
            if (ImPlot::BeginPlot("##Heatmap1", image_size, ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText)) {
                ImPlot::SetupAxes(nullptr, nullptr, axes_flags, axes_flags);
                ImPlot::PlotHeatmap("heat", image.GetImage(), image.nx, image.ny, scale_min, scale_max, nullptr,
                                    ImPlotPoint(0, 0), ImPlotPoint(1, 1));
                ImPlot::EndPlot();
            }
            ImGui::SameLine();
            ImPlot::ColormapScale("##HeatScale", scale_min, scale_max, ImVec2(60, image_size.y));
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("u")) {
            Image image_u(nghost, nx, ny, u);
            if (ImPlot::BeginPlot("##Heatmap1", image_size, ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText)) {
                ImPlot::SetupAxes(nullptr, nullptr, axes_flags, axes_flags);
                ImPlot::PlotHeatmap("heat", image_u.GetImage(), image.nx, image.ny, scale_min, scale_max, nullptr,
                                    ImPlotPoint(0, 0), ImPlotPoint(1, 1));
                ImPlot::EndPlot();
            }
            ImGui::SameLine();
            ImPlot::ColormapScale("##HeatScale", scale_min, scale_max, ImVec2(60, image_size.y));
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("v")) {
            Image image_v(nghost, nx, ny, v);
            if (ImPlot::BeginPlot("##Heatmap1", image_size, ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText)) {
                ImPlot::SetupAxes(nullptr, nullptr, axes_flags, axes_flags);
                ImPlot::PlotHeatmap("heat", image_v.GetImage(), image.nx, image.ny, scale_min, scale_max, nullptr,
                                    ImPlotPoint(0, 0), ImPlotPoint(1, 1));
                ImPlot::EndPlot();
            }
            ImGui::SameLine();
            ImPlot::ColormapScale("##HeatScale", scale_min, scale_max, ImVec2(60, image_size.y));
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("en")) {
            Image image_en(nghost, nx, ny, en);
            if (ImPlot::BeginPlot("##Heatmap1", image_size, ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText)) {
                ImPlot::SetupAxes(nullptr, nullptr, axes_flags, axes_flags);
                ImPlot::PlotHeatmap("heat", image_en.GetImage(), image.nx, image.ny, scale_min, scale_max, nullptr,
                                    ImPlotPoint(0, 0), ImPlotPoint(1, 1));
                ImPlot::EndPlot();
            }
            ImGui::SameLine();
            ImPlot::ColormapScale("##HeatScale", scale_min, scale_max, ImVec2(60, image_size.y));
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

Grid::Grid(RTSettings &settings) {
    Reset(settings);
}

void Grid::Reset(RTSettings &settings) {
    AttrsFromSettings(settings);
    Resize();
    RTInstability();
    PrimToCons();
}

void Grid::Clear() {
    delete reconstructor;
    delete riemann_solver;
}

void Grid::AttrsFromSettings(RTSettings &settings) {
    this->nx = settings.nx;
    this->ny = settings.ny;
    this->nghost = settings.nghost;
    this->reconstruct_type = settings.reconstruct_type;
    if (this->reconstruct_type == CONSTANT && this->nghost < 1) {
        this->nghost = 1;
        settings.nghost = 1;
    } else if (this->reconstruct_type == LINEAR && this->nghost < 2) {
        this->nghost = 2;
        settings.nghost = 2;
    }
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
    this->riemann_solver_type = settings.riemann_solver_type;
    // Delete the old reconstructor and create a new one
    this->reconstructor = new Reconstructor(nx, ny, nghost, reconstruct_type);
    this->riemann_solver = new RiemannSolver(nx, ny, nghost, riemann_solver_type);
    this->rkstages = settings.rkstages;
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
    for (int j = 0; j < ny; j++) {
        for (int i = 0; i < nx; i++) {
            const float xi = x1 + dlx * ((i + 1) - 0.5f);
            const float yj = y1 + dly * ((j + 1) - 0.5f);

            gx[i + nghost][j + nghost] = grav_x_ini;
            gy[i + nghost][j + nghost] = grav_y_ini;
            if (yj <= 0.0f) {
                rho[i + nghost][j + nghost] = rho_ini_lower;
            } else {
                rho[i + nghost][j + nghost] = rho_ini_upper;
            }
            en[i + nghost][j + nghost] = en_ini + gy[i + nghost][j + nghost] * yj * rho[i + nghost][j + nghost];
            u[i + nghost][j + nghost] = 0.0f;
            v[i + nghost][j + nghost] = perturb_strength * (1.0f + std::cos(4.0f * M_PI * xi)) * (
                                            1.0f + std::cos(3.0f * M_PI * yj)) /
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
            const float rho_new = cons.rho[i][j] > 0.0f ? cons.rho[i][j] : 1.0e-6f;
            u[i + nghost][j + nghost] = cons.u[i][j] / rho_new;
            v[i + nghost][j + nghost] = cons.v[i][j] / rho_new;
            en[i + nghost][j + nghost] =
                    (gamma_ad - 1) * (cons.en[i][j] - 0.5 / rho_new * (
                                          std::pow(cons.u[i][j], 2) + std::pow(cons.v[i][j], 2)));
            rho[i + nghost][j + nghost] = rho_new;
        }
    }
}

void Grid::TimeStep() {
    // Advance one time step
    PrimToCons();

    QVec2 cons0(nx, ny);
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            cons0.rho[i][j] = rho[i + nghost][j + nghost];
            cons0.u[i][j] = rho[i + nghost][j + nghost] * u[i + nghost][j + nghost];
            cons0.v[i][j] = rho[i + nghost][j + nghost] * v[i + nghost][j + nghost];
            cons0.en[i][j] = en[i + nghost][j + nghost] * (1 / (gamma_ad - 1))
                             + 0.5f * rho[i + nghost][j + nghost] * (std::pow(u[i + nghost][j + nghost], 2) +
                                                                     std::pow(v[i + nghost][j + nghost], 2));
        }
    }

    for (int it = 0; it < rkstages; it++) {
        // First, apply boundary conditions
        ApplyBoundaryConditions();

        QVec2 res(nx, ny);

        // Calculate the fluxes in x direction
        for (int j = 0; j < ny; j++) {
            QVec qlx(nx + 1), qrx(nx + 1), qx(nxg);
            QVec fluxx(nx + 1);
            for (int i = 0; i < nxg; i++) {
                qx.Set(i, rho[i][j + nghost], u[i][j + nghost], v[i][j + nghost], en[i][j + nghost]);
            }
            reconstructor->Reconstruct(qx, qlx, qrx, XDIR);
            riemann_solver->Solve(qlx, qrx, fluxx, gamma_ad, XDIR);
            for (int i = 0; i < nx; i++) {
                res.rho[i][j] += (fluxx.rho[i + 1] - fluxx.rho[i]) / dlx;
                res.u[i][j] += (fluxx.u[i + 1] - fluxx.u[i]) / dlx;
                res.v[i][j] += (fluxx.v[i + 1] - fluxx.v[i]) / dlx;
                res.en[i][j] += (fluxx.en[i + 1] - fluxx.en[i]) / dlx;
            }
        }

        // Calculate the fluxes in y direction
        for (int i = 0; i < nx; i++) {
            QVec qly(ny + 1), qry(ny + 1), qy(nyg);
            QVec fluxy(ny + 1);
            for (int j = 0; j < nyg; j++) {
                qy.Set(j, rho[i + nghost][j], u[i + nghost][j], v[i + nghost][j], en[i + nghost][j]);
            }
            reconstructor->Reconstruct(qy, qly, qry, YDIR);
            qly.FlipVelocities(1);
            qry.FlipVelocities(1);
            riemann_solver->Solve(qly, qry, fluxy, gamma_ad, YDIR);
            fluxy.FlipVelocities(-1);
            for (int j = 0; j < ny; j++) {
                res.rho[i][j] += (fluxy.rho[j + 1] - fluxy.rho[j]) / dly;
                res.u[i][j] += (fluxy.u[j + 1] - fluxy.u[j]) / dly;
                res.v[i][j] += (fluxy.v[j + 1] - fluxy.v[j]) / dly;
                res.en[i][j] += (fluxy.en[j + 1] - fluxy.en[j]) / dly;
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
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                cons.rho[i][j] = ALPHA[it][0] * cons0.rho[i][j] + ALPHA[it][1] * cons.rho[i][j] - ALPHA[it][2] * res.rho
                                 [i][j] * dt;
                cons.u[i][j] = ALPHA[it][0] * cons0.u[i][j] + ALPHA[it][1] * cons.u[i][j] - ALPHA[it][2] * res.u[i][j] *
                               dt;
                cons.v[i][j] = ALPHA[it][0] * cons0.v[i][j] + ALPHA[it][1] * cons.v[i][j] - ALPHA[it][2] * res.v[i][j] *
                               dt;
                cons.en[i][j] = ALPHA[it][0] * cons0.en[i][j] + ALPHA[it][1] * cons.en[i][j] - ALPHA[it][2] * res.en[i][
                                    j] * dt;
            }
        }
        ConsToPrim();
    }
}

void Grid::ApplyBoundaryConditions() {
    // Apply boundary conditions
    // Periodic in x, reflecting in y

    // x-direction
    for (int j = 0; j < nyg; j++) {
        for (int ig = 0; ig < nghost; ig++) {
            // Left boundary
            rho[ig][j] = rho[nx + ig][j];
            en[ig][j] = en[nx + ig][j];
            u[ig][j] = u[nx + ig][j];
            v[ig][j] = v[nx + ig][j];
            // Right boundary
            rho[nx + nghost + ig][j] = rho[nghost + ig][j];
            en[nx + nghost + ig][j] = en[nghost + ig][j];
            u[nx + nghost + ig][j] = u[nghost + ig][j];
            v[nx + nghost + ig][j] = v[nghost + ig][j];
        }
    }

    // y-direction
    for (int i = 0; i < nxg; i++) {
        for (int jg = 0; jg < nghost; jg++) {
            // Bottom boundary
            rho[i][nghost - 1 - jg] = rho[i][nghost + jg];
            en[i][nghost - 1 - jg] = en[i][nghost + jg];
            u[i][nghost - 1 - jg] = u[i][nghost + jg];
            v[i][nghost - 1 - jg] = -v[i][nghost + jg];
            // Top boundary
            rho[i][ny + nghost + jg] = rho[i][ny + nghost - 1 - jg];
            en[i][ny + nghost + jg] = en[i][ny + nghost - 1 - jg];
            u[i][ny + nghost + jg] = u[i][ny + nghost - 1 - jg];
            v[i][ny + nghost + jg] = -v[i][ny + nghost - 1 - jg];
        }
    }
}


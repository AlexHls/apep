#include "Grid.h"

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
    if (ImPlot::ColormapButton(ImPlot::GetColormapName(map),ImVec2(225,0),map)) {
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
    ImGui::DragFloatRange2("Min / Max",&scale_min, &scale_max, 0.01f, -20, 20);
    ImGui::Text("Current Time: %.3f", time);
    ImGui::Text("Current dlx: %.3f", dlx);
    ImGui::Text("Current dly: %.3f", dly);

    // Display image size
    ImGui::Text("Image Size: %.0f x %.0f", image_size.x, image_size.y);
    if (ImGui::Button("Print Grid")) {
        image.Print();
    }

    if (ImGui::Button("Save Grid")) {
        WriteGrid();
    }

    static ImPlotAxisFlags axes_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks;

    ImPlot::PushColormap(map);

    if (ImPlot::BeginPlot("##Heatmap1",image_size,ImPlotFlags_NoLegend|ImPlotFlags_NoMouseText)) {

        ImPlot::SetupAxes(nullptr, nullptr, axes_flags, axes_flags);
        ImPlot::PlotHeatmap("heat",image.GetImage(),image.nx,image.ny,scale_min,scale_max,nullptr,ImPlotPoint(0,0),ImPlotPoint(1,1));
        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ColormapScale("##HeatScale",scale_min, scale_max, ImVec2(60,image_size.y));
}

Grid::Grid(const struct RTSettings &settings) {
    Reset(settings);
}

void Grid::Reset(const RTSettings &settings) {
    AttrsFromSettings(settings);
    Resize();
    RTInstability();
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
            v[i][j] = perturb_strength * (1.0f + std::cos(4.0f*M_PI*xi)) * (1.0f + std::cos(3.0f*M_PI*yj)) / 4.0f;
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

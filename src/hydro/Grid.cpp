#include "Grid.h"
#include "Image.h"
#include "Settings.h"

#include "imgui.h"
#include "implot.h"

void Grid::Update() {
    Image image(nx, ny, rho);
    auto image_size = image.GetWindowSize();

    static float scale_min       = 0;
    static float scale_max       = 3.0f;

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

    static ImPlotAxisFlags axes_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks;

    ImPlot::PushColormap(map);

    if (ImPlot::BeginPlot("##Heatmap1",image_size,ImPlotFlags_NoLegend|ImPlotFlags_NoMouseText)) {

        ImPlot::SetupAxes(nullptr, nullptr, axes_flags, axes_flags);
        ImPlot::PlotHeatmap("heat",image.GetImage(),nx,ny,scale_min,scale_max,nullptr,ImPlotPoint(0,0),ImPlotPoint(1,1));
        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ColormapScale("##HeatScale",scale_min, scale_max, ImVec2(60,image_size.y));
}

Grid::Grid(const struct RTSettings &settings) {
    this->nx = settings.nx;
    this->ny = settings.ny;
    this->boundary_y = settings.boundary_y;
    this->rho_ini_lower = settings.rho_ini_lower;
    this->rho_ini_upper = settings.rho_ini_upper;

    rho.resize(nx);
    for (int i = 0; i < nx; i++) {
        rho[i].resize(ny);
        for (int j = 0; j < ny; j++) {
            if (j < boundary_y) {
                rho[i][j] = rho_ini_lower;
            } else {
                rho[i][j] = rho_ini_upper;
            }
        }
    }
}

void Grid::Reset(const RTSettings &settings) {
    // Essentially this is the same as the constructor, but we need to be able to reset the grid
    this->nx = settings.nx;
    this->ny = settings.ny;
    this->boundary_y = settings.boundary_y;
    this->rho_ini_lower = settings.rho_ini_lower;
    this->rho_ini_upper = settings.rho_ini_upper;

    rho.resize(nx);
    for (int i = 0; i < nx; i++) {
        rho[i].resize(ny);
        for (int j = 0; j < ny; j++) {
            if (j < boundary_y) {
                rho[i][j] = rho_ini_lower;
            } else {
                rho[i][j] = rho_ini_upper;
            }
        }
    }

}

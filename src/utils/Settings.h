#ifndef APEP_UTILS_SETTINGS_H
#define APEP_UTILS_SETTINGS_H

#include <imgui.h>

// This header includes the various settings structs that are used in the applications

struct RTSettings {
  int nx, ny, nghost; // Grid resolution
  float rho_ini_upper, rho_ini_lower; // Initial density of the two fluids
  float en_ini;
  float grav_x_ini, grav_y_ini; // Initial gravity
  float x1, x2, y1, y2; // Grid boundaries
  float perturb_strength; // Perturbation strength
  float tmax; // Maximum time
  float cfl; // CFL number
  float gamma_ad; // Adiabatic index
  int resetting;
  int playing;
  int advance;
  int cycles_per_frame;
  int reconstruct_type; // 0 for constant, 1 for linear
  int riemann_solver_type; // 0 for HLLE, 1 for HLLC
  int rkstages; // Number of Runge-Kutta stages
  RTSettings() {
    // Set default values
    nx = 5;
    ny = 15;
    nghost = 1;
    x1 = -0.25f;
    x2 = 0.25f;
    y1 = -0.75f;
    y2 = 0.75f;
    rho_ini_upper = 2.0f; // This is the density of the fluid on the top
    rho_ini_lower = 1.0f; // This is the density of the fluid on the bottom
    en_ini = 2.5f;
    grav_x_ini = -0.0f;
    grav_y_ini = -0.1f;
    perturb_strength = 0.01f;
    tmax = 10.0f;
    cfl = 0.8f;
    gamma_ad = 1.4f;
    resetting = 0;
    playing = 0;
    advance = 0;
    cycles_per_frame = 1;
    reconstruct_type = 1;
    riemann_solver_type = 1;
    rkstages = 2;
  }

  void Update() {
    ImGui::Begin("RT Instability Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::InputInt("nx", &nx);
    ImGui::InputInt("ny", &ny);
    ImGui::InputInt("nghost", &nghost);
    ImGui::InputFloat("rho_ini_upper", &rho_ini_upper);
    ImGui::InputFloat("rho_ini_lower", &rho_ini_lower);
    ImGui::InputFloat("en_ini", &en_ini);
    ImGui::InputFloat("grav_x_init", &grav_x_ini);
    ImGui::InputFloat("grav_y_init", &grav_y_ini);
    ImGui::InputFloat("perturb_strength", &perturb_strength);
    ImGui::InputFloat("x1", &x1);
    ImGui::InputFloat("x2", &x2);
    ImGui::InputFloat("y1", &y1);
    ImGui::InputFloat("y2", &y2);
    ImGui::InputFloat("tmax", &tmax);
    ImGui::InputFloat("cfl", &cfl);
    ImGui::InputFloat("gamma_ad", &gamma_ad);
    ImGui::SliderInt("cycles_per_frame", &cycles_per_frame, 1, 10);
    if (ImGui::CollapsingHeader("Reconstruction Method")) {
      const char *items[] = {"Constant", "Linear"};
      static int item_current = 1;
      if (ImGui::BeginListBox("Reconstruction Method")) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
          const bool is_selected = (item_current == n);
          if (ImGui::Selectable(items[n], is_selected)) {
            item_current = n;
            reconstruct_type = n;
          }
          if (is_selected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
      }
    }
    if (ImGui::CollapsingHeader("Riemann Solver")) {
      const char *items[] = {"HLLE", "HLLC"};
      static int item_current = 1;
      if (ImGui::BeginListBox("Riemann Solver")) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
          const bool is_selected = (item_current == n);
          if (ImGui::Selectable(items[n], is_selected)) {
            item_current = n;
            riemann_solver_type = n;
          }
          if (is_selected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
      }
    }
    if (ImGui::CollapsingHeader("Integrator")) {
      const char *items[] = {"Euler", "RK2"};
      static int item_current = 1;
      if (ImGui::BeginListBox("Integrator")) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
          const bool is_selected = (item_current == n);
          if (ImGui::Selectable(items[n], is_selected)) {
            item_current = n;
            rkstages = n + 1;
          }
          if (is_selected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
      }
    }
    if (ImGui::Button("Reset")) {
      resetting++;
    }
    if (ImGui::Button("Play/Pause")) {
      playing = !playing;
    }
    ImGui::SameLine();
    ImGui::Text("Playing: %s", playing ? "true" : "false");
    if (ImGui::Button("Advance")) {
      advance++;
    }
    ImGui::End();
  }
};

#endif //APEP_UTILS_SETTINGS_H

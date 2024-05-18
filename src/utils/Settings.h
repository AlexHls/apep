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
  int resetting;
  int playing;
  RTSettings() {
    // Set default values
    nx = 24;
    ny = 72;
    nghost = 2;
    x1=-0.25f;
    x2=0.25f;
    y1=-0.75f;
    y2=0.75f;
    rho_ini_upper = 1.0f; // This is the density of the fluid on the top
    rho_ini_lower = 3.0f; // This is the density of the fluid on the bottom
    en_ini = 2.5f;
    grav_x_ini = -0.0f;
    grav_y_ini = -0.1f;
    perturb_strength = 0.01f;
    tmax = 1.0f;
    resetting = 0;
    playing = 0;
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
    if (ImGui::Button("Reset")) {
      resetting++;
    }
    if (ImGui::Button("Play/Pause")) {
      playing = !playing;
    }
    ImGui::SameLine();
    ImGui::Text("Playing: %s", playing ? "true" : "false");
    ImGui::End();
  }
};

#endif //APEP_UTILS_SETTINGS_H

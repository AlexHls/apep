#ifndef APEP_UTILS_SETTINGS_H
#define APEP_UTILS_SETTINGS_H

#include <imgui.h>

// This header includes the various settings structs that are used in the applications

struct RTSettings {
  int nx, ny; // Grid resolution
  int boundary_y; // Location of the boundary between the two fluids
  float rho_ini_upper, rho_ini_lower; // Initial density of the two fluids
  int resetting;
  int playing;
  RTSettings() {
    // Set default values
    nx = 64;
    ny = 64;
    boundary_y = 32;
    rho_ini_upper = 1.0f; // This is the density of the fluid on the top
    rho_ini_lower = 3.0f; // This is the density of the fluid on the bottom
    resetting = 0;
    playing = 0;
  }
  void Update() {
    ImGui::Begin("RT Instability Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::InputInt("nx", &nx);
    ImGui::InputInt("ny", &ny);
    ImGui::InputInt("boundary_y", &boundary_y);
    ImGui::InputFloat("rho_ini_upper", &rho_ini_upper);
    ImGui::InputFloat("rho_ini_lower", &rho_ini_lower);
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

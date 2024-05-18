#include "implot.h"
#include "app/App.h"
#include "hydro/Grid.h"
#include "utils/Settings.h"

struct RTInstabilityApp : App {
  using App::App;
  RTSettings settings;
  Grid grid = Grid(settings);

  void Update() override {
    ImGui::Begin("Status", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    ImGuiIO &io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

    settings.Update();
    if (settings.resetting > 0) {
      grid.Reset(settings);
      settings.resetting = 0;
    }
    grid.Update();
  }
};
int main(int argc, char const *argv[]) {
  RTInstabilityApp app("Rayleigh-Taylor Instability", 1280, 800, argc, argv);
  app.Run();
  return EXIT_SUCCESS;
}

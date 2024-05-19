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
      grid.Clear();
      grid.Reset(settings);
      settings.resetting = 0;
    }
    grid.Update();
    if (grid.time < settings.tmax && settings.playing) {
      grid.TimeStep();
      grid.time += grid.dt;
    }
  }
};

int main(int argc, char const *argv[]) {
  RTInstabilityApp app("Rayleigh-Taylor Instability", 1920, 1080, argc, argv);
  app.Run();
  return EXIT_SUCCESS;
}

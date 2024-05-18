#include "implot.h"
#include "app/App.h"

struct RTInstabilityApp : App {
  using App::App;
  void Update() override {
    ImPlot::ShowDemoWindow();
  }
};
int main(int argc, char const *argv[]) {
  RTInstabilityApp app("Rayleigh-Taylor Instability", 1920, 1080, argc, argv);
  app.Run();
  return EXIT_SUCCESS;
}

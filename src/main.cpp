#include "imgui.h"
#include "implot.h"

#include "app/App.h"

struct TestApp : App {
  using App::App;
  void Update() override {
    ImPlot::ShowDemoWindow();
  }
};
int main(int argc, char const *argv[]) {
  TestApp app("Test Window", 1920, 1080, argc, argv);
  app.Run();
  return EXIT_SUCCESS;
}

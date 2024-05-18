#include <cmath>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <GLFW/glfw3.h>

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int argc, char *argv[]) {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    return EXIT_FAILURE;
  }

  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // Create window with graphics context
  GLFWwindow* window = glfwCreateWindow(1280, 720, "apep", nullptr, nullptr);
  if (window == nullptr) {
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

  // Set Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Our state
  bool show_output_window = true;
  bool show_plot_window = true;
  ImVec4 clear_color;
  clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  float a = 0.0f;
  float b = 1.0f;

  int ploy_degree = 1;

  float x[20] = {-10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    ImGui::Begin("Input window");

    ImGui::Text("Set the input for the calculation here");
    ImGui::Checkbox("Output Window", &show_output_window);
    ImGui::Checkbox("Plot Window", &show_plot_window);

    ImGui::SliderFloat("Input a", &a, 0.0f, 1.0f);
    ImGui::SliderFloat("Input b", &b, 1.0f, 10.0f);

    // Increment -/+ buttons to change the degree of the polynomial
    if (ImGui::Button("-")) {
      if (ploy_degree > 0) {
        ploy_degree--;
      }
    }
    ImGui::SameLine();
    ImGui::Text("Polynomial degree: %d", ploy_degree);
    ImGui::SameLine();
    if (ImGui::Button("+")) {
      ploy_degree++;
    }


    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

    if (show_output_window) {
      static float c = 0.0f;
      ImGui::Begin("Output window");

      ImGui::Text(("Output of a + b"));
      ImGui::Text("a: %.3f", a);
      ImGui::Text("b: %.3f", b);

      c = a + b;
      ImGui::Text("a + b = %.3f", c);


      ImGui::End();
    }

    if (show_plot_window) {
      static float f_x[20] = {-10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
      ImGui::Begin("Plot window");
      ImGui::Text("f(x)");

      for (int i = 0; i < 20; i++) {
        f_x[i] = a * std::pow(x[i], ploy_degree) + b;
      }
      if (ImPlot::BeginPlot("Result")) {
        ImPlot::PlotLine("f(x)", x, f_x, 20);
        ImPlot::EndPlot();
      }

      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);


  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}

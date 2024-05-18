#ifndef APEP_APP_APP_H
#define APEP_APP_APP_H
#include <glad/glad.h>
#include <imgui.h>
#include <map>
#include <string>
#include <GLFW/glfw3.h>


struct App {
    // Constructor
    App(std::string title,  int w, int h, int argc, char const *argv[]);
    // Destructor
    virtual ~App();
    // Called at top of run
    virtual void Start() {}
    // Update, called once per frame
    virtual void Update() {/* Default implementation does nothing */}
    // Runs the app
    void Run();
    // Get window size
    ImVec2 GetWindowSize();

    ImVec4 ClearColor;                      // background clear color
    GLFWwindow* Window;                     // GLFW window handle
    std::map<std::string,ImFont*> Fonts;    // font map
    bool UsingDGPU;                         // using discrete GPU (laptops only)
};



#endif //APEP_APP_APP_H

#ifndef EVENTS_H
#define EVENTS_H

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>

#include "../src/extern/imgui.h"
#include "../src/extern/imgui_impl_sdl2.h"
#include "../src/extern/imgui_impl_opengl3.h"

#include <string>
#include <iostream>

class events
{
public: 
    SDL_Event event;
    float deltaTime = 0.0f;
    int running = 1;
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    // for the camera / player pov
    int speed = 1;
    events(SDL_Event cur_event, SDL_Window * mainwindow, int * W_WIDTH, int * W_HEIGHT);
    void pollEvent();
    void mouseEvent();

private:
    SDL_Window *sdl_main_window;
    int * m_WIDTH; //window width
    int * m_HEIGHT; //window height
    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = 0.0f;
    float lastY = 0.0f;
    float fov = 45.0;
    int receiving_mouse_events = 1;
};

#endif
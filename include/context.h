#ifndef CONTEXT_H
#define CONTEXT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include <GL/glew.h>

#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class context{
public:
    context();
    void deleteContext();
    int W_WIDTH = 1200;
    int W_HEIGHT = 800;
    SDL_Window *mainwindow;
    SDL_GLContext maincontext;
private:
    
};


#endif
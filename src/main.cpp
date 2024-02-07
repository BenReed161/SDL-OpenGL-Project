#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include "extern/imgui.h"
#include "extern/imgui_impl_sdl2.h"
#include "extern/imgui_impl_opengl3.h"
#include <GL/glew.h>

#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/shader.h"
#include "../include/events.h"

#include <iostream>

#define NUMBEROFOBJS = 2;

void pollEvent(SDL_Event &, int &);
void mouseEvent(void);

unsigned int W_WIDTH = 1200;
unsigned int W_HEIGHT = 800;
unsigned int VBO, VAO = 0;

float lastFrame = 0.0f;

SDL_Window *mainwindow;
SDL_GLContext maincontext;

int main()
{   
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }
   
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, 
            SDL_GL_CONTEXT_PROFILE_CORE );

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    mainwindow = SDL_CreateWindow("SDL2 & OpenGL 3.3", SDL_WINDOWPOS_CENTERED, 
    SDL_WINDOWPOS_CENTERED, W_WIDTH, W_HEIGHT, 
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    
    if (!mainwindow) {
        printf("error creating the main window SDL ERR : %s\n", SDL_GetError());
        return 1;
    }

    maincontext = SDL_GL_CreateContext(mainwindow);

    if (!maincontext) {
        printf("error creating opengl context OPENGL ERR : %s\n", SDL_GetError());
        return 1;
    }

    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        printf( "Error initializing GLEW OPENGL ERR : %s\n", glewGetErrorString( glewError ) );
        return 1;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(mainwindow, maincontext);
    ImGui_ImplOpenGL3_Init();

    //shader code
    shader mainShader("./shaders/prog_vertex.vert", "./shaders/prog_fragment.frag");

    // Vertex buffers
    float vertices[] = {
        // positions         // colors
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 

        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,

         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,

        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f
    };
    //glm::vec3 positions[] = {
    //    glm::vec3( 0.0f, 0.0f, 0.0f ),
    //    glm::vec3( 0.0f, 0.0f, 3.0f )
    //};
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // render loop
    mainShader.use();

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)W_WIDTH / (float)W_HEIGHT, 0.1f, 100.0f);
    mainShader.setMat4("projection", projection);

    SDL_Event event;
    
    events eventSystem(event, mainwindow);

    while (eventSystem.running){
        // (Where your code calls SDL_PollEvent())
        ImGui_ImplSDL2_ProcessEvent(&eventSystem.event); // Forward your event to backend

        // (After event loop)
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        //?
        //SDL_Color color; https://stackoverflow.com/questions/22886500/how-to-render-text-in-sdl2
        
        // Main loop
        float currFrame = static_cast<float>(((float)SDL_GetTicks64())/1000);
        eventSystem.deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;

        // poll the mouse and keyboard/window events
        eventSystem.pollEvent(); //SDL2 PollEvent while loop
        eventSystem.mouseEvent();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Activate the shaders for our program
        mainShader.use();
        
        glm::mat4 view = glm::lookAt(eventSystem.cameraPos, eventSystem.cameraPos + eventSystem.cameraFront, eventSystem.cameraUp);
        mainShader.setMat4("view", view);

        
        glBindVertexArray(VAO);
        // render the static cubes
        glm::mat4 model = glm::mat4(1.0f);
        mainShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        
        // cube that follows camera
        // render the moving cubes
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, eventSystem.cameraPos);

        //rotate the model?? 
        //model2 = glm::rotate(model2, -cameraFront.x, cameraUp);        
        //model2 = glm::rotate(model2, cameraFront.y, glm::vec3(1,0,0));

        mainShader.setMat4("model", model2);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        //debug info 
        //printf("x: %f\n", cameraFront.x);
        //printf("y: %f\n", cameraFront.y);
        //printf("z: %f\n\n", cameraFront.z);

        SDL_GL_SwapWindow(mainwindow);

        // Rendering
        // (Your code clears your framebuffer, renders your other stuff etc.)
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // (Your code calls SDL_GL_SwapWindow() etc.)
    }
    
    // De-allocate resources
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(mainShader.gProgram);

    SDL_GL_DeleteContext(maincontext);
    SDL_DestroyWindow(mainwindow);
    SDL_Quit();
    return 0;
}
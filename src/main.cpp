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
#include "../include/editor.h"
#include "../include/context.h"
#include "../include/object.h"

#include <iostream>
#include <vector>

#define NUMBEROFOBJS = 2;

unsigned int VBO, VAO = 0;

float lastFrame = 0.0f;

int main()
{   
    //create the init class and init data
    context mainContext;

    //call the shader class - shader code
    shader mainShader("./shaders/prog_vertex.vert", "./shaders/prog_fragment.frag");

    //load the data from an object file.
    object cube_obj("./res/cube_clean.obj");
    float * vertices = cube_obj.loadobj();

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> scales;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 432/*Size of cube (change so it's dynamic to the prog)*/, vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // render loop
    mainShader.use();

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)mainContext.W_WIDTH / (float)mainContext.W_HEIGHT, 0.1f, 100.0f);
    mainShader.setMat4("projection", projection);

    SDL_Event event;
    
    // Create an event system for the program and main game loop.
    events eventSystem(event, mainContext.mainwindow, &mainContext.W_WIDTH, &mainContext.W_HEIGHT);

    glm::vec3 scale = glm::vec3(1.f, 1.f, 1.f);

    glEnable(GL_DEPTH_TEST);

    while (eventSystem.running){
 
        // Main loop
        float currFrame = static_cast<float>(((float)SDL_GetTicks64())/1000);
        eventSystem.deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;

        // poll the mouse and keyboard/window events
        eventSystem.pollEvent(); //SDL2 PollEvent while loop
        eventSystem.mouseEvent();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        // Show the EDITOR menu
        //ImGui::showEditorMenu(positions, scales);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Activate the shaders for our program
        mainShader.use();
        
        glm::mat4 view = glm::lookAt(eventSystem.cameraPos, eventSystem.cameraPos + eventSystem.cameraFront, eventSystem.cameraUp);
        mainShader.setMat4("view", view);

        
        glBindVertexArray(VAO);
        // render the static cubes
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, scale);
        mainShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // render the other cubes
        /*
        for(unsigned int i = 0; i < positions.size(); i++) {
            glm::mat4 editor = glm::mat4(1.0f);
            editor = glm::translate(editor, positions.at(i));
            editor = glm::scale(editor, scales.at(i));

            mainShader.setMat4("model", editor);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }*/

        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        
        // cube that follows camera
        // render the moving cubes
        glm::mat4 model2 = glm::mat4(1.0f);
        
        model2 = glm::translate(model2, glm::vec3(0.0f,0.0f,3.f));

        mainShader.setMat4("model", model2);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        /*
        
        */

        //debug
        //printf("x: %f\n", cameraFront.x);
        //printf("y: %f\n", cameraFront.y);
        //printf("z: %f\n\n", cameraFront.z);

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(mainContext.mainwindow);
    }

    // De-allocate resources
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(mainShader.gProgram);

    mainContext.deleteContext();
    
    SDL_Quit();

    return 0;
}
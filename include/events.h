#ifndef EVENTS_H
#define EVENTS_H

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>

#include <string>
#include <iostream>

class events
{
public: 

    float deltaTime = 0.0f;

    int running = 1;

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    
    // for the camera / player pov
    int speed = 1;

    events(SDL_Event cur_event, SDL_Window * mainwindow){
        event = cur_event;
        sdl_main_window = mainwindow;
    }

    void pollEvent() {
        while (SDL_PollEvent(&event)){
        // Event handler loop - keyboard - exit
            switch(event.type){   
                case SDL_QUIT:
                {
                    printf("Closing..\n");
                    running = 0;
                    break;
                }
                case SDL_KEYDOWN:
                {
                    switch ( event.key.keysym.sym )
                    {
                        case SDLK_ESCAPE:
                        {
                            printf("Closing..\n");
                            running = 0;
                            break;
                        }
                        case SDLK_LSHIFT:
                        {
                            speed = 5;
                            break;
                        }
                    }
                    break;
                }
                case SDL_KEYUP:
                {
                    switch ( event.key.keysym.sym )
                    {
                        case SDLK_LSHIFT:
                        {
                            speed = 1;
                            break;
                        }
                    }
                    break;
                }
                default:
                    break;
            }
            switch (event.window.event) 
            {
                case SDL_WINDOWEVENT_RESIZED:
                {
                    SDL_SetWindowSize(sdl_main_window, event.window.data1, event.window.data2);
                    W_WIDTH = event.window.data1;
                    W_HEIGHT = event.window.data2;
                    glViewport(0,0,W_WIDTH, W_HEIGHT);
                    break;
                }
                
                default:
                    break;
            }
        }
        float cameraSpeed = static_cast<float>(2.5 * deltaTime * speed);

        const Uint8* kb = SDL_GetKeyboardState(NULL);
        cameraPos += (kb[SDL_SCANCODE_W] * cameraSpeed * cameraFront) - (kb[SDL_SCANCODE_S] * cameraSpeed * cameraFront);
        glm::vec3 normal = glm::vec3(glm::normalize(glm::cross(cameraFront, cameraUp)));
        cameraPos += (kb[SDL_SCANCODE_D] * cameraSpeed * normal) - (kb[SDL_SCANCODE_A] * cameraSpeed * normal);// - (kb[SDL_SCANCODE_A] * normal * cameraSpeed);
    }

    void mouseEvent(void) {
        int xpos = 0;
        int ypos = 0;
        SDL_GetRelativeMouseState(&xpos, &ypos);

        if (xpos == lastX && ypos == lastY) { // no mouse movments detected
            return;
        }

        lastX = xpos;
        lastY = ypos;

        float sense = 0.1f;
        yaw += xpos * sense;
        pitch += -ypos * sense;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }

private:
    SDL_Event event;
    SDL_Window *sdl_main_window;

    unsigned int W_WIDTH = 1200; //inwdow width
    unsigned int W_HEIGHT = 800; //window height

    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = W_WIDTH / 2.0;
    float lastY = W_HEIGHT / 2.0;
    float fov = 45.0;
};

#endif
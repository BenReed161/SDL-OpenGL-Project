#include "../include/events.h"

events::events(SDL_Event cur_event, SDL_Window * mainwindow, int * W_WIDTH, int * W_HEIGHT){
    m_WIDTH = W_WIDTH;
    m_HEIGHT = W_HEIGHT;
    event = cur_event;
    sdl_main_window = mainwindow;
}

void events::pollEvent() {
    while (SDL_PollEvent(&event)){
    // Event handler loop - keyboard - exit
        // Process IMGUI input events and window resizing.
        ImGui_ImplSDL2_ProcessEvent(&event);

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
                        printf("Exiting..\n");
                        running = 0;
                        break;
                    }
                    case SDLK_LSHIFT:
                    {
                        speed = 5;
                        break;
                    }
                    case SDLK_e:
                    {
                        printf("Tabbing to edit..\n");
                        receiving_mouse_events = !receiving_mouse_events;
                        SDL_SetRelativeMouseMode((SDL_bool)receiving_mouse_events);
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
                m_WIDTH = &event.window.data1;
                m_HEIGHT = &event.window.data2;
                glViewport(0,0,*m_WIDTH, *m_HEIGHT);
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
};

void events::mouseEvent(void) {
    if(!receiving_mouse_events)
        return;

    int xpos = 0;
    int ypos = 0;
    SDL_GetRelativeMouseState(&xpos, &ypos);

    if (xpos == lastX && ypos == lastY) { // NO MOUSE MOVEMENTS DETECTED - Return as this functions logic is wasted in this case
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
};
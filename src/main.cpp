#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include <GL/glew.h>

#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#define NUMBEROFOBJS = 2;

void pollEvent(SDL_Event &, int &);
void mouseEvent(void);

unsigned int W_WIDTH = 1200;
unsigned int W_HEIGHT = 800;
unsigned int gProgram = 0;
unsigned int VBO, VAO = 0;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = W_WIDTH / 2.0;
float lastY = W_HEIGHT / 2.0;
float fov = 45.0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

SDL_Window *mainwindow;
SDL_GLContext maincontext;
//SDL_Renderer *renderer;

char * readfromFile(char * name) {
    /*
    
    FILE * fileptr;
    fileptr = fopen(name, "r");
    if (!fileptr) {
        printf("ERR : Attempting to read file name %s File does not exist.\n", name);
        exit(1);
    }
    fseek(fileptr, 0, SEEK_END);
    
    int size = ftell(fileptr);
    
    rewind(fileptr);
    //fclose(fileptr);
    //fileptr = fopen(name, "r");

    char * str = new char[ size ];
    
    fread(str, 1, size, fileptr);
    fclose(fileptr);
    
    //
    strcat(str, "\0");
    printf("%s\n", str);
    return str;
    */
    FILE* fileptr = fopen(name, "r");
    if (!fileptr) {
        printf("ERR : Attempting to read file name %s File does not exist.\n", name);
        exit(1);
    }
    // Determine file size
    if((fseek(fileptr, 0, SEEK_END))) {
        printf("ERR : Failed to seek into file %s\n", name);
        exit(1);
    }
    size_t size = ftell(fileptr);

    char* str = new char[size];

    rewind(fileptr);
    fread(str, sizeof(char), size, fileptr);
    fclose(fileptr);
    
    printf("%s\n", str);
    //strcat(str, "\0");
    return str;
}

/*
void render_text(
    SDL_Renderer * renderer,
    int x,
    int y, 
    const char * text,
    TTF_Font * font,
    SDL_Rect * rect,
    SDL_Color * color
) {
    SDL_Surface * surface;
    SDL_Texture * texture;

    surface = TTF_RenderText_Solid(font, text, *color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect->x = x;
    rect->y = y;
    rect->w = surface->w;
    rect->h = surface->h;

    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, rect);
    SDL_DestroyTexture(texture);
}*/
/* This is wasteful for textures that stay the same.
    * But makes things less stateful and easier to use.
    * Not going to code an atlas solution here... are we? */

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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        printf( "Error initializing GLEW OPENGL ERR : %s\n", glewGetErrorString( glewError ) );
        return 1;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);

    //glEnable(GL_DEPTH_TEST);

    char s[] = "./shaders/prog_vertex.vert";
    char * file = readfromFile(s);
    const GLchar* vertexShaderSource[] = {file};

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    delete file;
    // fragment shader
    char s1[] = "./shaders/prog_fragment.frag";
    file = readfromFile(s1);
    const GLchar* fragmentShaderSource[] = {file};

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    delete file;
    // link shaders
    unsigned int gProgram = glCreateProgram();
    glAttachShader(gProgram, vertexShader);
    glAttachShader(gProgram, fragmentShader);
    glLinkProgram(gProgram);
    // check for linking errors
    glGetProgramiv(gProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(gProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
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
    glm::vec3 positions[] = {
        glm::vec3( 0.0f, 0.0f, 0.0f ),
        glm::vec3( 0.0f, 0.0f, 3.0f )
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // render loop
    // -----------
    glUseProgram(gProgram);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)W_WIDTH / (float)W_HEIGHT, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(gProgram, "projection\0"), 1, GL_FALSE, glm::value_ptr(projection));
    //printf("projection : %d\n", glGetUniformLocation(gProgram, "projection\0"));

    /*
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("mono.ttf", 24);
    if (font == NULL) {
        printf("ERR : font not found\n");
        exit(1);
    }
    */

    int running = 1;
    while (running){
        //SDL_Color color; https://stackoverflow.com/questions/22886500/how-to-render-text-in-sdl2
        //color.r = 255;
        //color.g = 255;
        //color.b = 255;
        //color.a = 255;
        //SDL_Rect rect;
        
        // Main loop
        float currFrame = static_cast<float>(((float)SDL_GetTicks64())/1000);
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;
        
        SDL_Event event;

        pollEvent(event, running);
        mouseEvent();

        //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        //SDL_RenderClear(renderer);
        //render_text(renderer, 0, 0, "FPS: ", font, &rect, &color);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(gProgram);
        
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(gProgram, "view\0"), 1, GL_FALSE, glm::value_ptr(view));

        
        glBindVertexArray(VAO);
        // render the static cubes
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(gProgram, "model\0"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        // render the moving cubes
        for (unsigned int i = 0; i < 1; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cameraPos);

            model = glm::rotate(model, cameraFront.x * -1, cameraUp);
            model = glm::rotate(model, cameraFront.y, glm::vec3(1,0,0));

            glUniformMatrix4fv(glGetUniformLocation(gProgram, "model\0"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        //printf("x: %f\n", cameraUp.x);
        //printf("y: %f\n", cameraUp.y);
        //printf("z: %f\n\n", cameraUp.z);

        SDL_GL_SwapWindow(mainwindow);
    }
    
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(gProgram);

    SDL_GL_DeleteContext(maincontext);
    SDL_DestroyWindow(mainwindow);
    SDL_Quit();
    return 0;
}

void pollEvent(SDL_Event & event, int & running) {
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
                }
            }
            
            default:
                break;
        }
        switch (event.window.event) 
        {
            case SDL_WINDOWEVENT_RESIZED:
            {
                SDL_SetWindowSize(mainwindow, event.window.data1, event.window.data2);
                W_WIDTH = event.window.data1;
                W_HEIGHT = event.window.data2;
                glViewport(0,0,W_WIDTH, W_HEIGHT);
                break;
            }
            
            default:
                break;
        }
    }
    float cameraSpeed = static_cast<float>(2.5 * deltaTime);

    const Uint8* kb = SDL_GetKeyboardState(NULL);
    cameraPos += (kb[SDL_SCANCODE_W] * cameraSpeed * cameraFront) - (kb[SDL_SCANCODE_S] * cameraSpeed * cameraFront);
    glm::vec3 normal = glm::vec3(glm::normalize(glm::cross(cameraFront, cameraUp)));
    cameraPos += (kb[SDL_SCANCODE_D] * cameraSpeed * normal) - (kb[SDL_SCANCODE_A] * cameraSpeed * normal);// - (kb[SDL_SCANCODE_A] * normal * cameraSpeed);
}

void mouseEvent(void) {
    int xpos = 0;
    int ypos = 0;
    SDL_GetMouseState(&xpos, &ypos);
    if (xpos == lastX && ypos == lastY) { // no mouse movments detected
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sense = 0.1f;
    xoffset *= sense;
    yoffset *= sense;

    yaw += xoffset;
    pitch += yoffset;

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
//g++ -o shaders ./shaders_interpolation.cpp -lGL -lGLU -lglfw -lGLEW

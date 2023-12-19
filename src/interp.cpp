#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int W_WIDTH = 800;
const unsigned int W_HEIGHT = 600;
unsigned int gProgram = 0;
unsigned int VBO, VAO = 0;

/*
const char *vtexShaderSource ="#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
    "   ourColor = aColor;\n"
    "}\0";

const char *fmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0f);\n"
    "}\n\0";

*/

void pollEvent(SDL_Event &, int &);

char * readfromFile(char * name) {
    FILE * fileptr;
    fileptr = fopen(name, "r");
    if (!fileptr) {
        printf("ERR : Attempting to read file name %s File does not exist.\n", name);
        
        exit(1);
    }
    fseek(fileptr, 0L, SEEK_END);
    
    int size = ftell(fileptr) + 1;
    
    //rewind(fileptr);
    fclose(fileptr);
    fileptr = fopen(name, "r");

    char * str = new char[ size ];
    
    fread(str, 1, size, fileptr);
    fclose(fileptr);
    
    //printf("%s", str);
    strcat(str, "\n\0");
    return str;
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    SDL_Window *mainwindow;
    SDL_GLContext maincontext;
    
    
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
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    
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

    char s[] = "./shaders/vert_test.vert";
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
    char s1[] = "./shaders/frag_test.frag";
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
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 

    };

    
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
    // -----------
    glUseProgram(gProgram);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)W_WIDTH / (float)W_HEIGHT, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(gProgram, "projection\0"), 1, GL_FALSE, glm::value_ptr(projection));
    printf("projection : %d\n", glGetUniformLocation(gProgram, "projection\0"));

    int running = 1;
    while (running){
        // Main loop
        
        SDL_Event event;

        pollEvent(event, running);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(gProgram);

        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        float radius = 10.0f;
        float camX = static_cast<float>(sin( ((float)SDL_GetTicks64())/1000 ) * radius);
        float camZ = static_cast<float>(cos( ((float)SDL_GetTicks64())/1000 ) * radius);
        view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(gProgram, "view\0"), 1, GL_FALSE, glm::value_ptr(view));

        // render the triangle
        glBindVertexArray(VAO);

        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(gProgram, "model\0"), 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLES, 0, 3);

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
                    case SDLK_e:
                    {
                        printf("key: e\n");
                    }
                } 
            }
        }
    }
}

//g++ -o shaders ./shaders_interpolation.cpp -lGL -lGLU -lglfw -lGLEW
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int W_HEIGHT = 500;
int W_WIDTH = 750;


glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.f);
glm::vec3 cameraFront = glm::vec3(0.f, 0.f, -1.f);
glm::vec3 cameraUp = glm::vec3(0.f, 1.f, 0.f);


GLuint gProgramID = 0;
GLuint gVertexShader = 0;
GLuint gFragmentShader = 0;
GLuint gVertexPos2DLocation = -1;
GLuint gVBO = 0;
GLuint gIBO = 0;

char * readfromFile(char * name) {
    FILE * fileptr;
    fileptr = fopen(name, "r");
    if (!fileptr) {
        printf("ERR : Attempting to read file name %s File does not exist.\n", name);
        
        exit(1);
    }
    fseek(fileptr, 0L, SEEK_END);
    
    int size = ftell(fileptr)+1;
    
    rewind(fileptr);
    
    char * str = new char[ size ];
    
    fread(str, 1, size-1, fileptr);
    fclose(fileptr);

    return str;
}

void printProgramLog( GLuint program )
{
    //Make sure name is shader
    if( glIsProgram( program ) )
    {
        //Program log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;
        
        //Get info string length
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );
        
        //Allocate string
        char* infoLog = new char[ maxLength ];
            
        //Get info log
        glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 )
        {
            //Print Log
            printf( "%s\n", infoLog );
        }
        
        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        printf( "Name %d is not a program\n", program );
    }
}

void printShaderLog( GLuint shader )
{
    //Make sure name is shader
    if( glIsShader( shader ) )
    {
        //Shader log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;
        
        //Get info string length
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );
        
        //Allocate string
        char* infoLog = new char[ maxLength ];

        //Get info log
        glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 )
        {
            //Print Log
            printf( "%s\n", infoLog );
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        printf( "Name %d is not a shader\n", shader );
    }
}

void render()
{
    
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );
    
    //Render quad
    //Bind program
    glUseProgram( gProgramID );

    //Enable vertex position
    glEnableVertexAttribArray( gVertexPos2DLocation );

    //Set vertex data
    glBindBuffer( GL_ARRAY_BUFFER, gVBO );
    glVertexAttribPointer( gVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL );

    //Set index data and render
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
    glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );

    //Disable vertex position
    glDisableVertexAttribArray( gVertexPos2DLocation );

    //Unbind program
    glUseProgram( 0x0 );
    
}

int initGL()
{
    //Success flag
    int success = 1;

    //Generate program
    gProgramID = glCreateProgram();
   //Create vertex shader
    gVertexShader = glCreateShader( GL_VERTEX_SHADER );

    //Get vertex source
    char s[] = "./shaders/vert_test.vert";
    char * file = readfromFile(s);
    const GLchar* vertexShaderSource[] = {file};
    //Set vertex source
    glShaderSource( gVertexShader, 1, vertexShaderSource, NULL );
    delete [] file; // Free the memory associated with the opened file.
    //Compile vertex source
    glCompileShader( gVertexShader );

    //Check vertex shader for errors
    GLint vShaderCompiled = GL_FALSE;
    glGetShaderiv( gVertexShader, GL_COMPILE_STATUS, &vShaderCompiled );
    if( vShaderCompiled != GL_TRUE )
    {
        printf( "Unable to compile vertex shader %d!\n", gVertexShader );
        printShaderLog( gVertexShader );
        success = 0;
    }
    else
    {
        //Attach vertex shader to program
        glAttachShader( gProgramID, gVertexShader );


        //Create fragment shader
        gFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

        //Get fragment source
        char s1[] = "./shaders/frag_test.frag";
        file = readfromFile(s1);
        const GLchar* fragmentShaderSource[] = {file};

        //Set fragment source
        glShaderSource( gFragmentShader, 1, fragmentShaderSource, NULL );
        delete [] file; // Free the memory associated with the opened file.

        //Compile fragment source
        glCompileShader( gFragmentShader );

        //Check fragment shader for errors
        GLint fShaderCompiled = GL_FALSE;
        glGetShaderiv( gFragmentShader, GL_COMPILE_STATUS, &fShaderCompiled );
        if( fShaderCompiled != GL_TRUE )
        {
            printf( "Unable to compile fragment shader %d!\n", gFragmentShader );
            printShaderLog( gFragmentShader );
            success = 0;
        }
        else
        {
            //Attach fragment shader to program
            glAttachShader( gProgramID, gFragmentShader );


            //Link program
            glLinkProgram( gProgramID );

            //Check for errors
            GLint programSuccess = GL_TRUE;
            glGetProgramiv( gProgramID, GL_LINK_STATUS, &programSuccess );
            if( programSuccess != GL_TRUE )
            {
                printf( "Error linking program %d!\n", gProgramID );
                printProgramLog( gProgramID );
                success = 0;
            }
            else
            {
                //Get vertex attribute location
                //gVertexPos2DLocation = glGetAttribLocation( gProgramID, "LVertexPos2D" );
                //if( gVertexPos2DLocation == -1 )
                //{
                //    printf( "LVertexPos2D is not a valid glsl program variable!\n" );
                //    success = 0;
                //}
                //else
                //{
                    //Initialize clear color
                    glClearColor( 0.f, 0.f, 0.f, 1.f );

                    //VBO data
                    GLfloat vertexData[] =
                    {
                        // front
                        -0.5f, -0.5f,
                         0.5f, -0.5f,
                         0.5f,  0.5f,
                        -0.5f,  0.5f,
                        // left
                    };
                    
                    //IBO data
                    GLuint indexData[] = { 0, 1, 2, 3};

                    //Create VBO
                    glGenBuffers( 1, &gVBO );
                    glBindBuffer( GL_ARRAY_BUFFER, gVBO );
                    glBufferData( GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW );

                    //Create IBO
                    glGenBuffers( 1, &gIBO );
                    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
                    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW );
                //}
            }
        }
    }
    
    return success;
}

int main(int argc, char *argv[]){
    
    SDL_Window *mainwindow;
    SDL_GLContext maincontext;
    
    
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }
   
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, 
            SDL_GL_CONTEXT_PROFILE_CORE );

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    mainwindow = SDL_CreateWindow("sdl", SDL_WINDOWPOS_CENTERED, 
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
    else {
        glewExperimental = GL_TRUE;
        GLenum glewError = glewInit();
        if (glewError != GLEW_OK) {
            printf( "Error initializing GLEW OPENGL ERR : %s\n", glewGetErrorString( glewError ) );
            return 1;
        }

        //Init OpenGL
        if( !initGL() ) {
            printf( "Unable to initialize OpenGL\n" );
            return 1;
        }
    }

    //VSYNC
    SDL_GL_SetSwapInterval(1);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)W_WIDTH / (float)W_HEIGHT, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(gVertexShader, "projection"), 1, GL_FALSE, &projection[0][0]);
    
    int running = 1;
    while (running){
        // Main loop
        
        SDL_Event event;

        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        float radius = 10.0f;
        float camX = static_cast<float>(sin(SDL_GetTicks64()) * radius);
        float camZ = static_cast<float>(cos(SDL_GetTicks64()) * radius);
        view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(gVertexShader, "view"), 1, GL_FALSE, &view[0][0]);

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
        render();
        SDL_GL_SwapWindow(mainwindow);
    }
    
    SDL_GL_DeleteContext(maincontext);
    
    SDL_DestroyWindow(mainwindow);
    
    SDL_Quit();
}

// Sources :
// https://lazyfoo.net/tutorials/SDL/51_SDL_and_modern_opengl/index.php

// https://learnopengl.com/Getting-started/Camera

// https://lazyfoo.net/tutorials/OpenGL/36_vertex_array_objects/index.php

// https://lazyfoo.net/tutorials/SDL/51_SDL_and_modern_opengl/index.php

// https://www.khronos.org/opengl/wiki/Tutorial2:_VAOs,_VBOs,_Vertex_and_Fragment_Shaders_(C_/_SDL)

// Ideas :
// https://www.khronos.org/opengl/wiki/VBO_-_just_examples
// https://openglbook.com/chapter-3-index-buffer-objects-and-primitive-types.html
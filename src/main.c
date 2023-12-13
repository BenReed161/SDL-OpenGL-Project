#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
/*
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glext.h>
*/
#include <GL/glu.h>

#include <stdio.h>
#include <stdlib.h>

int W_HEIGHT = 500;
int W_WIDTH = 750;

GLuint gProgramID = 0;
GLuint gVertexPos2DLocation = -1;
GLuint gVBO = 0;
GLuint gIBO = 0;

char * readfromFile(char* name) {
    FILE * fileptr;
    fileptr = fopen(name, "r");
    if (!fileptr) {
        printf("ERR : Attempting to read file name %s File does not exist.\n", name);
        
        exit(1);
    }
    fseek(fileptr, 0L, SEEK_END);
    
    int size = ftell(fileptr)+1;
    
    rewind(fileptr);
    
    char * str = memset(malloc(size), '\0', size);
    
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
        char* infoLog = malloc(sizeof(char) * maxLength);
        
        //Get info log
        glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 )
        {
            //Print Log
            printf( "%s\n", infoLog );
        }
        
        //Deallocate string
        free(infoLog);
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
        char* infoLog = malloc(sizeof(char) * maxLength);
        
        //Get info log
        glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 )
        {
            //Print Log
            printf( "%s\n", infoLog );
        }

        //Deallocate string
        free(infoLog);
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
    GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );

    //Get vertex source
    char * file = readfromFile("./shaders/vert_test.vert");
    const GLchar* vertexShaderSource[] = {file};
    //Set vertex source
    glShaderSource( vertexShader, 1, vertexShaderSource, NULL );
    free(file); // Free the memory associated with the opened file.
    //Compile vertex source
    glCompileShader( vertexShader );

    //Check vertex shader for errors
    GLint vShaderCompiled = GL_FALSE;
    glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &vShaderCompiled );
    if( vShaderCompiled != GL_TRUE )
    {
        printf( "Unable to compile vertex shader %d!\n", vertexShader );
        printShaderLog( vertexShader );
        success = 0;
    }
    else
    {
        //Attach vertex shader to program
        glAttachShader( gProgramID, vertexShader );


        //Create fragment shader
        GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

        //Get fragment source
        file = readfromFile("./shaders/frag_test.frag");
        const GLchar* fragmentShaderSource[] = {file};

        //Set fragment source
        glShaderSource( fragmentShader, 1, fragmentShaderSource, NULL );
        free(file); // Free the memory associated with the opened file.

        //Compile fragment source
        glCompileShader( fragmentShader );

        //Check fragment shader for errors
        GLint fShaderCompiled = GL_FALSE;
        glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled );
        if( fShaderCompiled != GL_TRUE )
        {
            printf( "Unable to compile fragment shader %d!\n", fragmentShader );
            printShaderLog( fragmentShader );
            success = 0;
        }
        else
        {
            //Attach fragment shader to program
            glAttachShader( gProgramID, fragmentShader );


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
                gVertexPos2DLocation = glGetAttribLocation( gProgramID, "LVertexPos2D" );
                if( gVertexPos2DLocation == -1 )
                {
                    printf( "LVertexPos2D is not a valid glsl program variable!\n" );
                    success = 0;
                }
                else
                {
                    //Initialize clear color
                    glClearColor( 0.f, 0.f, 0.f, 1.f );

                    //VBO data
                    GLfloat vertexData[] =
                    {
                        -0.5f, -0.5f,
                         0.5f, -0.5f,
                         0.5f,  0.5f,
                        -0.5f,  0.5f
                    };

                    //IBO data
                    GLuint indexData[] = { 0, 1, 2, 3 };

                    //Create VBO
                    glGenBuffers( 1, &gVBO );
                    glBindBuffer( GL_ARRAY_BUFFER, gVBO );
                    glBufferData( GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW );

                    //Create IBO
                    glGenBuffers( 1, &gIBO );
                    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
                    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW );
                }
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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
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

    //unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    //glClearColor ( 1.0, 0.0, 0.0, 1.0 );
    //glClear ( GL_COLOR_BUFFER_BIT );
    
    int running = 1;
    while (running){
        // Main loop
        
        SDL_Event event;

        while (SDL_PollEvent(&event)){
        // Event handler loop - keyboard - exit

            switch(event.type){
                
                case SDL_QUIT:
                {
                    printf("Closing..\n");
                    running = 0;
                    break;
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
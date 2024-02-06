#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>

class shader
{
public: 
    unsigned int gProgram;
    
    shader(const char * vert_file, const char * frag_file){
        // vertex shader
        char * file = readfromFile(vert_file);
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
        file = readfromFile(frag_file);
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
        gProgram = glCreateProgram();
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
    }

    void use() const 
    {
        glUseProgram(gProgram);
    }

    void setMat4(const std::string &name, const glm::mat4 &matrix) const
    {
        glUniformMatrix4fv(glGetUniformLocation(gProgram, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
    }

private:
    char * readfromFile(const char * name) {
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
        printf("Read file \"%s\" correctly\n\n", name);
        
        return str;
    }
};

#endif
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
    shader(const char * vert_file, const char * frag_file);
    void setMat4(const std::string &name, const glm::mat4 &matrix);
    void use();

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
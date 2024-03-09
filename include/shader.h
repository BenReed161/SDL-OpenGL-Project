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
    char * readfromFile(const char * name);
};

#endif
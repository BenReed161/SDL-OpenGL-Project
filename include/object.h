#ifndef OBJECT_H
#define OBJECT_H

#include <string>

class object {
    public:
        object(std::string);
        ~object();
        float * loadobj();
    private:
        std::string filename;
        float* vertices;
};

#endif
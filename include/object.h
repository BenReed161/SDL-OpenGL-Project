#ifndef OBJECT_H
#define OBJECT_H

#include <string>

class object {
    public:
        object(std::string);
        ~object();
        float * loadobj();
        size_t size();
    private:
        std::string filename;
        float* vertices;
        size_t size_total;
};

#endif
#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>

class object {
    public:
        object(std::string);
        ~object();
        float * loadobj();
        size_t face_count();
        size_t size();
    private:
        std::string filename;
        float* vertices;
        size_t size_total;
        std::vector<float> vert_arr;
        std::vector<int> face_arr;
};

#endif
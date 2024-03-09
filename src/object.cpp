#include "../include/object.h"

#include <fstream>
#include <string>
#include <iostream>
#include <vector>

object::object(std::string file_name) {
    //loop throughout the file if line starts with v, capture the next data with space as delimter adding it to an array
    //load the vertex data
    //load the face data
    //if say "f 1 3 5"
    //array index 0 of the vertex data concat to the list + index 2 and index 4 (subtract 1 from each index of the array)
    //
    //use the face data to create an array of vertices for the opengl format - faces of tris.
    //return a float array
    filename = file_name;
}

object::~object(){
    std::cout << "deconstructed obj" << std::endl;
    delete vertices;
}

float * object::loadobj() {
    std::ifstream obj_file(filename);
    if (!obj_file.is_open()) {
        std::cout << "ERR : Failed to seek into file " << filename << std::endl;
        exit(1);
    }
    std::string line;

    std::vector<float> vert_arr;
    std::vector<int> face_arr;

    while (std::getline(obj_file, line)) {
        std::string curr_vert;
        if (line[0] == 118){
            for (size_t i = 2; i < line.size(); i++) {
                if (line[i] != 32)
                    curr_vert.push_back(line[i]);
                else {
                    vert_arr.push_back(std::stof(curr_vert));
                    curr_vert.clear();
                }
            }
            vert_arr.push_back(std::stof(curr_vert));
            curr_vert.clear();
        }
        if (line[0] == 102){
            for (size_t i = 2; i < line.size(); i+=2) {
                face_arr.push_back(line[i]-48);
            }
        }
    }

    vertices = new float[face_arr.size() * 3];

    int j = 0;
    for (std::vector<int>::iterator it = face_arr.begin() ; it != face_arr.end(); ++it){
        int x = 0;
        for(int i = j; i < (j+3); i++){
            vertices[i]=vert_arr[(((*it - 1) * 3) + x)];
            x++;
        }
        j+=3;
    }

    obj_file.close();

    return vertices;
}
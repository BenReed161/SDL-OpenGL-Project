#include "../include/object.h"

#include <fstream>
#include <string>
#include <iostream>
#include <vector>



object::object(std::string file_name) {

    filename = file_name;
}

object::~object() {
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
	int x = 0;
	int total_size = 0;
	int total_faces = 0;
	// Get the amount of faces
	while (std::getline(obj_file, line)) {
		if (line[0] == 102){
			total_faces++;
		}
	}
	obj_file.clear();
	obj_file.seekg(0);

    // faces * ((6) * 3)
	vertices = new float[total_faces * 18];
    
	while (std::getline(obj_file, line)) {
        std::string curr_vert;
		std::string curr_norm;
        std::string curr_face;
        if (line[0] == 118 && line[1] == 32){
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
			total_size += 3;
        }
		else if (line[0] == 118 && line[1] == 110) {
			for (size_t i = 3; i < line.size(); i++) {
				if (line[i] != 32)
					curr_norm.push_back(line[i]);
				else {
					norm_arr.push_back(std::stof(curr_norm));
					curr_norm.clear();
				}
			}
			norm_arr.push_back(std::stof(curr_norm));
			curr_norm.clear();
			total_size += 3;
		}
        else if (line[0] == 102) {
			
			int inc = 0;
			for (size_t i = 2; i < line.size(); i++) {
				inc = 0;	
				// Set the vertex data
				if (line[i] == 47 && line[i+1] == 47) {
					for (int a = x; a < (x+3); a++) {
						vertices[a]=vert_arr[((std::stoi(curr_face)-1)*3)+inc];
						inc++;
					}
					x+=3;
					curr_face.clear();
				}
				// Set the normal data
				else if(line[i] == 32) {
					for (int a = x; a < (x+3); a++) {
						vertices[a]=norm_arr[((std::stoi(curr_face)-1)*3)+inc];
						inc++;
					}
					x+=3;
					curr_face.clear();
				}
				else if(line[i] != 32 && line[i] != 47) {
					curr_face.push_back(line[i]);
				}
			}
			//Account for the final Normal at the end of the line.
			for (int a = x; a < (x+3); a++) {
				vertices[a]=norm_arr[((std::stoi(curr_face)-1)*3)+inc];
				inc++;
			}
			x+=3;
			curr_face.clear();
			faces++;
        }
    }

    //DEBUG
//    for (std::vector<float>::iterator it = norm_arr.begin() ; it != norm_arr.end(); ++it){
//        std::cout << (*it) << std::endl;
//    }

	std::cout << "----------" << std::endl;    
    //DEBUG
//    for (std::vector<float>::iterator it = vert_arr.begin() ; it != vert_arr.end(); ++it){
//        std::cout << (*it) << std::endl;
//    }
    
    //DEBUG
//	std::cout << faces << std::endl;
//    for (int i = 0; i < faces * 6; i++){
//        std::cout << vertices[i] << ", ";
//    }
	vert_arr.clear();
	face_arr.clear();
	norm_arr.clear();

    obj_file.close();

    // each face has 3 verticies + 3 normals and each index is 4 bytes.
	std::cout << "total_size:" << total_size << std::endl;
    size_total = total_faces * 18 * 4;

    return vertices;
}

size_t object::face_count() {
    return faces;
}

size_t object::size() {
    return size_total;
}

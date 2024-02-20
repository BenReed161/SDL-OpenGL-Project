#ifndef EDITOR_H
#define EDITOR_H

#include "../src/extern/imgui.h"

#include <glm/glm.hpp>

#include <vector>

namespace ImGui {
    void showEditorMenu(std::vector<glm::vec3> & positions, std::vector<glm::vec3> & scales);
}

#endif
#ifndef EDITOR_H
#define EDITOR_H

#include "../src/extern/imgui.h"

#include <glm/glm.hpp>

namespace ImGui {
    void showEditorMenu(glm::vec3 positions[]);
}

#endif
#include "./extern/imgui.h"
#include "../include/editor.h"

#include <glm/glm.hpp>

#include <math.h>

#include <iostream>
#include <vector>

void ImGui::showEditorMenu(float & positions, ImVec4 & color ) {
    ImGui::Begin("Editor");
	ImGui::Text("Light options:");
	ImGui::SliderFloat("light-pos", (float*)&positions, -1.0f, 1.0f);
	ImGui::ColorEdit3("color-edit", (float*)&color);
    ImGui::End();
}

#include "./extern/imgui.h"
#include "../include/editor.h"

#include <glm/glm.hpp>

#include <math.h>

#include <iostream>
#include <vector>

void ImGui::showEditorMenu(std::vector<glm::vec3> & positions, std::vector<glm::vec3> & scales) {
    ImGui::Begin("Editor");
    if (ImGui::BeginTabBar("##TabBar"))
    {
        if (ImGui::BeginTabItem("Top"))
        {
            //float scale = 1.0f;

            static std::vector<glm::vec2> points;
            static glm::vec2 scrolling(0.0f, 0.0f);
            static bool adding_line = false;

            ImGui::Text("Mouse Left: drag to add lines,\nMouse Right: drag to scroll, click for context menu.");

            // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
            ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
            ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
            if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
            if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
            ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

            // Draw border and background color
            ImGuiIO& io = ImGui::GetIO();
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
            draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

            // This will catch our interactions
            ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
            const bool is_hovered = ImGui::IsItemHovered(); // Hovered
            const bool is_active = ImGui::IsItemActive();   // Held
            const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
            
            float xDistance = io.MousePos.x - origin.x;
            float yDistance = io.MousePos.y - origin.y;
            float mouse_pos_x;
            float mouse_pos_y;
            /*
            if((int)(xDistance) % 64 != 0){
                if ((xDistance) >= 0)
                    mouse_pos_x = 64 * ((int)(xDistance)/64);
                else
                    mouse_pos_x = -64 * (-1*((int)(xDistance)/64));
                
                if ((yDistance) >= 0)
                    mouse_pos_y = 64 * ((int)(yDistance)/64);
                else
                    mouse_pos_y = -64 * (-1*((int)(yDistance)/64));
            }
            else {*/
            mouse_pos_x = xDistance;
            mouse_pos_y = yDistance;
            //}
            const glm::vec2 mouse_pos_in_canvas(mouse_pos_x, mouse_pos_y);

            //Add point at origin.
            draw_list->AddCircle(ImVec2(origin.x, origin.y), 3, IM_COL32(255, 0, 0, 255));

            // Add first and second point
            glm::vec2 mouse_pos_on_click;
            if (is_hovered && !adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                mouse_pos_on_click = mouse_pos_in_canvas;
                points.push_back(mouse_pos_in_canvas);
                points.push_back(mouse_pos_in_canvas);
                adding_line = true;
            }
            if (adding_line)
            {
                points.back() = mouse_pos_in_canvas;

                float xdiff = (mouse_pos_on_click.x - mouse_pos_in_canvas.x) / 2; //Difference between the position clicked and the end position of the mouse
                float ydiff = ((mouse_pos_on_click.y - mouse_pos_in_canvas.y) / 2)-600;
                
                //printf("mouse_pos: %f, %f\n\n", mouse_pos_in_canvas.x, mouse_pos_in_canvas.y);
                //printf("diffx: %f,  diffy: %f\n", xdiff, ydiff);
                if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
                {
                    adding_line = false;
                    scales.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
                    positions.push_back(glm::vec3(-1*xdiff/64.0, 0.0f, -1*ydiff/64.0));
                    //printf("x:%f , y:%f\n", mouse_pos_in_canvas.x/64.0, mouse_pos_in_canvas.y/64.0);
                    //printf("scalex:%f , scaley:%f\n", scaleX, scaleY);
                }
            }

            // Pan (we use a zero mouse threshold when there's no context menu)
            // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
            const float mouse_threshold_for_pan = -1.0f;
            if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
            {
                scrolling.x += io.MouseDelta.x;
                scrolling.y += io.MouseDelta.y;
            }

            // Context menu (under default mouse threshold)
            ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
            if (drag_delta.x == 0.0f && drag_delta.y == 0.0f)
                ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
            if (ImGui::BeginPopup("context"))
            {
                if (adding_line)
                    points.resize(points.size() - 2);
                adding_line = false;
                if (ImGui::MenuItem("Remove one", NULL, false, points.size() > 0)) { 
                    points.resize(points.size() - 2); 
                }
                if (ImGui::MenuItem("Remove all", NULL, false, points.size() > 0)) { 
                    points.clear();
                    positions.clear();
                }
                ImGui::EndPopup();
            }

            // Draw grid + all lines in the canvas
            draw_list->PushClipRect(canvas_p0, canvas_p1, true);

            const float GRID_STEP = 64.0f;
            
            for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
                draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
            for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
                draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
            
            // Draw square
            for (unsigned int n = 0; n < points.size(); n += 2) {
                //draw_list->AddCircle(ImVec2((origin.x + points[n].x) -, (origin.y + points[n].y)), 3, IM_COL32(0, 255, 0, 255));
                draw_list->AddRect(ImVec2(origin.x + points[n].x, origin.y + points[n].y), ImVec2(origin.x + points[n + 1].x, origin.y + points[n + 1].y), IM_COL32(255, 255, 0, 255));
            }
 
            draw_list->PopClipRect();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::Text("Hello from another window!");
    ImGui::Button("Test button");
    ImGui::End();
}

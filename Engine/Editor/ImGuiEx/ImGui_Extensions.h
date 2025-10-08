#pragma once
#include "../Core/Core.h"
#include "../Core/pch.h"

namespace Iaonnis {
	namespace ImGuiEx
	{
        static bool Button(const char* label, ImVec2 size, ImDrawFlags flags = ImDrawFlags_RoundCornersNone)
        {
            static int _id = 1;

            
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImVec2 end = ImVec2(pos.x + size.x, pos.y + size.y);

            bool pressed = ImGui::InvisibleButton(label, size);

            ImU32 col = ImGui::GetColorU32(
                pressed ? ImGuiCol_ButtonActive :
                ImGui::IsItemHovered() ? ImGuiCol_ButtonHovered :
                ImGuiCol_Button
            );

            drawList->AddRectFilled(pos, end, col, 7.0f, flags);

            ImVec2 textSize = ImGui::CalcTextSize(label);
            ImVec2 textPos = ImVec2(
                pos.x + (size.x - textSize.x) * 0.5f,
                pos.y + (size.y - textSize.y) * 0.5f
            );

            drawList->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), label);
            

            return pressed;
        }

        static bool ImageButton(const char* label, ImTextureID id, ImVec2 size, ImDrawFlags flags = ImDrawFlags_RoundCornersNone)
        {
            static int _id = 1;


            ImDrawList* drawList = ImGui::GetWindowDrawList();

            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImVec2 end = ImVec2(pos.x + size.x, pos.y + size.y);

            bool pressed = ImGui::InvisibleButton(label, size);

            ImU32 col = ImGui::GetColorU32(
                pressed ? ImGuiCol_ButtonActive :
                ImGui::IsItemHovered() ? ImGuiCol_ButtonHovered :
                ImGuiCol_Button
            );

            drawList->AddRectFilled(pos, end, col, 7.0f, flags);

            ImVec2 textSize = ImGui::CalcTextSize(label);
            ImVec2 textPos = ImVec2(
                pos.x + (size.x - textSize.x) * 0.5f,
                pos.y + (size.y - textSize.y) * 0.5f
            );

            drawList->AddImage(id, pos, end, ImVec2(1.0, 0), ImVec2(0, 1.0));

            return pressed;
        }

        // Vec3 InputFloat with colored labels
        static bool InputFloat3(const char* label, float* v, float resetValue = 0.0f,
            float columnWidth = 100.0f, const char* format = "%.3f")
        {
            bool modified = false;

            ImGui::PushID(label);
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text("%s", label);
            ImGui::NextColumn();

            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

            // X Component (Red)
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
            if (ImGui::Button("X", buttonSize))
            {
                v[0] = resetValue;
                modified = true;
            }
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            if (ImGui::DragFloat("##X", &v[0], 0.1f, 0.0f, 0.0f, format))
                modified = true;
            ImGui::PopItemWidth();
            ImGui::SameLine();

            // Y Component (Green)
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
            if (ImGui::Button("Y", buttonSize))
            {
                v[1] = resetValue;
                modified = true;
            }
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            if (ImGui::DragFloat("##Y", &v[1], 0.1f, 0.0f, 0.0f, format))
                modified = true;
            ImGui::PopItemWidth();
            ImGui::SameLine();

            // Z Component (Blue)
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
            if (ImGui::Button("Z", buttonSize))
            {
                v[2] = resetValue;
                modified = true;
            }
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            if (ImGui::DragFloat("##Z", &v[2], 0.1f, 0.0f, 0.0f, format))
                modified = true;
            ImGui::PopItemWidth();

            ImGui::PopStyleVar();
            ImGui::Columns(1);
            ImGui::PopID();

            return modified;
        }

        // Vec3 Slider with colored labels
        static bool SliderFloat3(const char* label, float* v, float min, float max,
            float resetValue = 0.0f, float columnWidth = 100.0f,
            const char* format = "%.3f")
        {
            bool modified = false;

            ImGui::PushID(label);
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text("%s", label);
            ImGui::NextColumn();

            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

            // X Component (Red)
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
            if (ImGui::Button("X", buttonSize))
            {
                v[0] = resetValue;
                modified = true;
            }
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            if (ImGui::SliderFloat("##X", &v[0], min, max, format))
                modified = true;
            ImGui::PopItemWidth();
            ImGui::SameLine();

            // Y Component (Green)
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
            if (ImGui::Button("Y", buttonSize))
            {
                v[1] = resetValue;
                modified = true;
            }
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            if (ImGui::SliderFloat("##Y", &v[1], min, max, format))
                modified = true;
            ImGui::PopItemWidth();
            ImGui::SameLine();

            // Z Component (Blue)
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
            if (ImGui::Button("Z", buttonSize))
            {
                v[2] = resetValue;
                modified = true;
            }
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            if (ImGui::SliderFloat("##Z", &v[2], min, max, format))
                modified = true;
            ImGui::PopItemWidth();

            ImGui::PopStyleVar();
            ImGui::Columns(1);
            ImGui::PopID();

            return modified;
        }


        static void SeperatorText(const char* label, float thickness, const ImVec2& alignment, const ImVec2& padding)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextAlign, alignment);
            ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextBorderSize, 5.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextPadding, padding);

            ImGui::SeparatorText(label);

            ImGui::PopStyleVar(3);
        }
       
    }
}
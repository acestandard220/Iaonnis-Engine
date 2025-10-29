#pragma once
#include "../Core/Core.h"
#include "../Core/pch.h"


namespace Iaonnis {
	namespace ImGuiEx
	{
        static const ImVec2 tightButtonPadding = ImVec2(1.5f, 1.5f);

        static void VSeparator()
        {
            ImGui::SameLine();
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
            ImGui::SameLine();
        }

        static void OnWindowHoverHightlight()
        {
            if (ImGui::IsWindowHovered())
            {
                auto windowRounding = ImGui::GetStyle().WindowRounding;
                ImGuiWindow* window = ImGui::GetCurrentWindow();
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                draw_list->Flags &= ~ImDrawListFlags_AntiAliasedLines;
                ImVec2 min = window->Pos;
                ImVec2 max = ImVec2(min.x + window->Size.x, min.y + window->Size.y);
                draw_list->AddRect(min, max, IM_COL32(255, 255, 255, 90), windowRounding, ImDrawFlags_RoundCornersAll, 2.0f);
            }
        }

        static bool OnItemHovered(std::function<void()> callback, float delay = 0.8f)
        {
            static float hoveredTime = 0.0f;
            
            if (ImGui::IsItemHovered())
            {
                hoveredTime += ImGui::GetIO().DeltaTime;
                if (hoveredTime > delay)
                {
                    ImGui::BeginTooltip();
                    callback();
                    ImGui::EndTooltip();
                }
                return true;
            }
            else {
                hoveredTime = 0.0f;
            }

            return false;
        }

        static bool Combo(const char* label, int* selection, const char* items_seperator_by_zeros, float width, bool enabled = true)
        {
            ImGui::SetNextItemWidth(width);

            ImGui::BeginDisabled(!enabled);
            bool changed =  ImGui::Combo(label, selection, items_seperator_by_zeros);
            ImGui::EndDisabled();

            return changed;
        }

        static bool TreeNode(const char* label, ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_Framed, std::function<void()> contentFunction = nullptr, float rounding = 5.0f,ImDrawFlags drawFlags = 256)
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->ChannelsSplit(2);

            ImVec2 start = ImGui::GetCursorScreenPos();
            float fullWidth = ImGui::GetContentRegionAvail().x;

            draw_list->ChannelsSetCurrent(1);
            bool opened = ImGui::TreeNodeEx(label, treeFlags);

            if (opened) 
            {
                if(contentFunction)
                    contentFunction();
                ImGui::Dummy(ImVec2(0, 5));

                ImVec2 end = ImGui::GetItemRectMax();
                end.x = start.x + fullWidth;

                draw_list->ChannelsSetCurrent(0);
                draw_list->AddRectFilled(
                    start,
                    end,
                    ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Header]),
                    rounding,drawFlags
                );

                draw_list->ChannelsMerge();
                ImGui::TreePop();
            }
            return opened;
        }

        static bool InputText(const char* label, char* buffer, int size, float width, bool enable, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None)
        {
            ImGui::BeginDisabled(!enable);
            ImGui::SetNextItemWidth(width);
            std::string text(size,' ');
            sprintf_s(text.data(), size, buffer);

            ImVec2 textSize = ImGui::CalcTextSize(buffer);

            int visibleSize = size;
            while (textSize.x > width)
            {
                visibleSize--;
                textSize = ImGui::CalcTextSize(text.substr(0, visibleSize).c_str());
            }

            bool tooLong = (visibleSize != size);
            if (tooLong)
            {
                visibleSize--;
                text = text.substr(0, visibleSize) + "...";
            }  

            bool results = ImGui::InputText(label, text.data(), text.size(), flags);
            if (tooLong && OnItemHovered([&]() {
                ImGui::Text(buffer);
            }));
            ImGui::EndDisabled();
            return results;
        }

        static bool Button(const char* label, ImVec2 size, float rounding = 5.0f, ImDrawFlags flags = ImDrawFlags_RoundCornersNone,bool enabled = true, ImVec2 padding = ImVec2(8, 4))
        {
            ImGui::BeginDisabled(!enabled);
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            ImVec2 pos = ImGui::GetCursorScreenPos();

            ImVec2 end = ImVec2(pos.x + size.x, pos.y + size.y);

            bool pressed = ImGui::InvisibleButton(label, size);

            ImU32 col = ImGui::GetColorU32(
                pressed ? ImGuiCol_ButtonActive :
                ImGui::IsItemHovered() ? ImGuiCol_ButtonHovered :
                ImGuiCol_Button
            );

            drawList->AddRectFilled(pos, end, col, rounding, flags);

            ImVec2 textSize = ImGui::CalcTextSize(label);
            ImVec2 textPos = ImVec2(
                pos.x + padding.x + (size.x - textSize.x - padding.x * 2) * 0.5f,
                pos.y + padding.y + (size.y - textSize.y - padding.y * 2) * 0.5f
            );

            drawList->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), label);

            ImGui::EndDisabled();
            return pressed;
        }
         


        static bool ImageButton(const char* label, ImTextureID id, ImVec2 size, float rounding = 5.0f, ImDrawFlags flags = ImDrawFlags_RoundCornersNone, ImVec2 padding = ImVec2(8, 4))
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            ImVec2 pos = ImGui::GetCursorScreenPos();

            ImVec2 end = ImVec2(pos.x + size.x, pos.y + size.y);

            bool pressed = ImGui::InvisibleButton(label, size);

            ImU32 col = ImGui::GetColorU32(
                pressed ? ImGuiCol_ButtonActive :
                ImGui::IsItemHovered() ? ImGuiCol_ButtonHovered :
                ImGuiCol_Button
            );

            drawList->AddRectFilled(pos, end, col, rounding, flags);

            // Shrink the image area by padding
            ImVec2 imgMin = ImVec2(pos.x + padding.x, pos.y + padding.y);
            ImVec2 imgMax = ImVec2(end.x - padding.x, end.y - padding.y);

            drawList->AddImage(id, imgMin, imgMax, ImVec2(0, 1),ImVec2(1, 0));

            return pressed;
        }

        static bool ImageSelectable(const char* label, ImTextureID id, ImVec2 size,
            bool selected = false,
            float rounding = 5.0f,
            ImDrawFlags flags = ImDrawFlags_RoundCornersNone,
            ImVec2 padding = ImVec2(8, 4))
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImVec2 end = ImVec2(pos.x + size.x, pos.y + size.y);

            // Disable selectable’s own background drawing
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

            bool pressed = ImGui::Selectable(label, selected, ImGuiSelectableFlags_None, size);

            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);

            // Pick your own highlight colors
            ImU32 col = ImGui::GetColorU32(
                selected ? ImVec4(0.2f, 0.6f, 1.0f, 0.5f) :  
                ImGui::IsItemHovered() ? ImVec4(1.0f, 1.0f, 1.0f, 0.15f) :  
                ImGui::GetStyleColorVec4(ImGuiCol_Button)  
            );

            // Draw your rounded background
            drawList->AddRectFilled(pos, end, col, rounding, flags);

            // Draw the image
            ImVec2 imgMin = ImVec2(pos.x + padding.x, pos.y + padding.y);
            ImVec2 imgMax = ImVec2(end.x - padding.x, end.y - padding.y);
            drawList->AddImage(id, imgMin, imgMax, ImVec2(0, 1), ImVec2(1, 0));

            return pressed;
        }

        static bool TextSelectable(const char* label, ImVec2 size,
            bool selected = false,
            float rounding = 5.0f,
            ImDrawFlags flags = ImDrawFlags_RoundCornersNone,
            ImVec2 padding = ImVec2(8, 4))
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImVec2 end = ImVec2(pos.x + size.x, pos.y + size.y);

            // Disable selectable's own background drawing
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

            std::string _label = std::string("##") + label ;
            bool pressed = ImGui::Selectable(_label.c_str(), selected, ImGuiSelectableFlags_None, size);

            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);

            // Pick your own highlight colors
            ImU32 col = ImGui::GetColorU32(
                selected ? ImVec4(0.2f, 0.6f, 1.0f, 0.5f) :
                ImGui::IsItemHovered() ? ImVec4(1.0f, 1.0f, 1.0f, 0.15f) :
                ImGui::GetStyleColorVec4(ImGuiCol_Button)
            );

            // Draw rounded background
            drawList->AddRectFilled(pos, end, col, rounding, flags);

            // Draw the text centered
            ImVec2 textSize = ImGui::CalcTextSize(label);
            ImVec2 textPos = ImVec2(
                pos.x + padding.x + (size.x - padding.x * 2 - textSize.x) * 0.5f,
                pos.y + padding.y + (size.y - padding.y * 2 - textSize.y) * 0.5f
            );

            ImU32 textCol = ImGui::GetColorU32(ImGuiCol_Text);
            drawList->AddText(textPos, textCol, label);

            return pressed;
        }

        //Same as XYZ below
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


        static bool InputFloat3Ex(const char* label, float* v, const char* p,
            float resetValue = 0.0f, float columnWidth = 100.0f, const char* format = "%.3f");

        static bool InputFloat3RGB(const char* label, float* v, float resetValue = 0.0f,
            float columnWidth = 100.0f, const char* format = "%.3f")
        {
            const char rgb[3] = { 'R', 'G', 'B' };
            return InputFloat3Ex(label, v, rgb, resetValue, columnWidth, format);
        }

        static bool InputFloat3XYZ(const char* label, float* v, float resetValue = 0.0f,
            float columnWidth = 100.0f, const char* format = "%.3f")
        {
            const char xyz[3] = { 'X', 'Y', 'Z' };
            return InputFloat3Ex(label, v, xyz, resetValue, columnWidth, format);
        }

        static bool InputFloat3Ex(const char* label, float* v, const char* p,
            float resetValue, float columnWidth, const char* format)
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

            std::string holder(1, p[0]);
            // X component
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
            if (ImGui::Button(holder.c_str(), buttonSize))
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

            holder = p[1];
            // Y component
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
            if (ImGui::Button(holder.c_str(), buttonSize))
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

            holder = p[2];
            // Z component
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
            if (ImGui::Button(holder.c_str(), buttonSize))
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
       
        static void PlotLines(const char* label, float* valueHistory,int& offset, int historyCapacity, float newValue, ImVec2 yRange, ImVec2 size)
        {
            ImGui::Text(label);
            valueHistory[offset] = newValue;
            offset = (offset + 1) % historyCapacity;

            float avg = 0.0f;
            for (int i = 0; i < historyCapacity; i++)
                avg += valueHistory[i];
            avg /= historyCapacity;

            char overlay[64];
            snprintf(overlay, sizeof(overlay), "Avg: %.1f", avg);

            ImGui::PlotLines("##Empty", valueHistory, historyCapacity, offset, overlay, yRange.x, yRange.y, size);
        }

        struct ToggleState2
        {
            ToggleState2(ImTextureID tex1, ImTextureID tex2, ImDrawFlags flags1 = 0, ImDrawFlags flags2 = 0)
            {
                textureIDs[0] = tex1;
                textureIDs[1] = tex2;

                drawFlags[0] = flags1;
                drawFlags[1] = flags2;
            }

            int GetToggleState() const { return currentState; }

            void OnRender(const char* label, ImVec2 buttonSize, float rounding = 7.0f, bool vertical = false, ImVec2 padding = ImVec2(8.0f, 4.0f))
            {
                for (int i = 0; i < 2; i++)
                {
                    std::string buttonLabel = label + std::to_string(i);
                    bool selected = (currentState == i);

                    if (ImGuiEx::ImageSelectable(buttonLabel.c_str(),
                        textureIDs[i], buttonSize, selected, rounding, drawFlags[i], padding))
                    {
                        currentState = i;
                    }

                    if (!vertical)
                        ImGui::SameLine();
                }
            }

        private:
            int currentState = 0;

            ImTextureID textureIDs[2];
            ImDrawFlags drawFlags[2];
        };

        struct ToggleGroup
        {
            ToggleGroup(const char* label, int nElements)
            {
                states.resize(nElements);
            }

            bool OnRender(int index,const char* label, bool* state, ImTextureID textureID, ImVec2 buttonSize, float rounding = 7.0f, ImDrawFlags drawFlag = 256)
            {
                bool result = (ImGuiEx::ImageSelectable(label, textureID, buttonSize, *state, rounding, drawFlag));
                if (result)
                {
                    *state = !(*state);
                }

                return result;
            }

        private:

            bool Toggle(int index)
            {
                states[index] = states[index] ? false : true;
                return states[index];
            }

        private:
            std::vector<bool> states;
        };

        static bool ToggleImageButton(const char* label, bool* state, ImTextureID textureID, ImVec2 buttonSize,
            float rounding = 7.0f, ImDrawFlags drawFlag = 256, ImVec2 padding = ImVec2(8.0f, 4.0f))
        {
            bool result = (ImGuiEx::ImageSelectable(label, textureID, buttonSize, *state, rounding, drawFlag,padding));
            if (result)
            {
                *state = !(*state);
            }
            return result;
        }

        
        static bool ToggleImageButtonState2(const char* label, bool* state1, bool* state2,
            ImTextureID tex1, ImTextureID tex2, ImVec2 buttonSize,
            float rounding = 5.0f, ImDrawFlags drawFlag1 = 0,
            ImDrawFlags drawFlag2 = 0, ImVec2 padding = ImVec2(0, 0))
        {
            char toggleLabel1[64];
            snprintf(toggleLabel1, sizeof(toggleLabel1), "%s0", label);
            bool result1 = ImGuiEx::ImageSelectable(toggleLabel1, tex1, buttonSize, *state1, rounding, drawFlag1, padding);

            char toggleLabel2[64];
            snprintf(toggleLabel2, sizeof(toggleLabel2), "%s1", label);
            bool result2 = ImGuiEx::ImageSelectable(toggleLabel2, tex2, buttonSize, *state2, rounding, drawFlag2, padding);

            if (result1)
            {
                *state1 = !(*state1);
                *state2 = false;  
                return true;
            }

            if (result2)
            {
                *state2 = !(*state2);
                *state1 = false; 
                return true;
            }

            return false;
        }

        static int ToggleTextButtonGroup(const char* labels,
            int* selectedIndex, int count,
            ImVec2 buttonSize,
            float rounding = 5.0f,
            ImDrawFlags* drawFlags = nullptr,
            ImVec2 padding = ImVec2(0, 0))
        {
            int activated = -1;
            const char* label = labels;

            for (int i = 0; i < count && label && *label; ++i)
            {
                ImGui::PushID(i);
                bool selected = (*selectedIndex == i);

                if (ImGuiEx::TextSelectable(label, buttonSize, selected, rounding,
                    drawFlags ? drawFlags[i] : 0, padding))
                {
                    *selectedIndex = i;
                    activated = i;
                }

                label += strlen(label) + 1; 
                ImGui::PopID();
            }

            return activated; 
        }

    }
}
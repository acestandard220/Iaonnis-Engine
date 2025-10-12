#pragma once
#include "../Core/pch.h"

namespace Iaonnis
{
    namespace ImGuiEx
    {
        namespace Fonts
        {
            static ImFont* regular = nullptr;
            static ImFont* bold = nullptr;
            static ImFont* light = nullptr;
        }

        static void SetBlenderTheme()
        {
            ImGuiStyle& style = ImGui::GetStyle();
            ImVec4* colors = style.Colors;

            // Blender's exact color palette (RGB values converted to 0-1 range)
            const ImVec4 blenderDarkGray = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);      // #292929 - Main panels
            const ImVec4 blenderMediumGray = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);    // #363636 - Secondary panels
            const ImVec4 blenderLightGray = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);     // #454545 - Headers
            const ImVec4 blenderVeryDarkGray = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);  // #1D1D1D - Title bars
            const ImVec4 blenderAlmostBlack = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);   // #101010 - Deepest areas

            // Blender orange accent colors
            const ImVec4 blenderOrange = ImVec4(0.98f, 0.55f, 0.18f, 1.00f);        // #FA8D2E - Selection/Active
            const ImVec4 blenderOrangeHover = ImVec4(1.00f, 0.65f, 0.28f, 1.00f);   // #FF9F47 - Hover
            const ImVec4 blenderOrangeActive = ImVec4(0.88f, 0.45f, 0.08f, 1.00f);  // #E07314 - Pressed

            // Blender blue for certain UI elements
            const ImVec4 blenderBlue = ImVec4(0.39f, 0.59f, 0.93f, 1.00f);          // #6397ED - Info/Links
            const ImVec4 blenderBlueHover = ImVec4(0.49f, 0.69f, 1.00f, 1.00f);     // #7DB0FF - Blue hover

            // Blender text colors
            const ImVec4 blenderWhite = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);         // #E6E6E6 - Main text
            const ImVec4 blenderGrayText = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);      // #ABABAB - Secondary text
            const ImVec4 blenderDarkText = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);      // #808080 - Disabled text

            // Blender border and separator colors
            const ImVec4 blenderBorder = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);        // Black borders
            const ImVec4 blenderSeparator = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);     // #595959 - Separators

            // Window backgrounds - Blender's panel system
            colors[ImGuiCol_WindowBg] = blenderDarkGray;                             // Main panels
            colors[ImGuiCol_ChildBg] = blenderMediumGray;                           // Sub-panels
            colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.13f, 0.13f, 0.98f);         // Popups/menus
            colors[ImGuiCol_Border] = blenderBorder;                                // Panel borders
            colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);    // No shadow

            // Title bars - very dark like Blender headers
            colors[ImGuiCol_TitleBg] = blenderVeryDarkGray;
            colors[ImGuiCol_TitleBgActive] = blenderLightGray;                      // Active window header
            colors[ImGuiCol_TitleBgCollapsed] = blenderVeryDarkGray;
            colors[ImGuiCol_MenuBarBg] = blenderVeryDarkGray;                       // Menu bar

            // Scrollbars - minimal like Blender
            colors[ImGuiCol_ScrollbarBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.53f);
            colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabActive] = blenderOrange;                   // Orange when dragged

            // Interactive elements - Blender orange theme
            colors[ImGuiCol_CheckMark] = blenderOrange;
            colors[ImGuiCol_SliderGrab] = blenderOrange;
            colors[ImGuiCol_SliderGrabActive] = blenderOrangeActive;

            // Buttons - flat Blender style
            colors[ImGuiCol_Button] = blenderMediumGray;
            colors[ImGuiCol_ButtonHovered] = blenderLightGray;
            colors[ImGuiCol_ButtonActive] = blenderOrangeActive;

            // Headers - collapsible sections like Blender properties
            colors[ImGuiCol_Header] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);          // #3D3D3D
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);   // Hover
            colors[ImGuiCol_HeaderActive] = blenderOrange;                          // Active/selected

            // Separators - Blender panel dividers
            colors[ImGuiCol_Separator] = blenderSeparator;
            colors[ImGuiCol_SeparatorHovered] = blenderOrange;
            colors[ImGuiCol_SeparatorActive] = blenderOrangeActive;

            // Resize grips - subtle
            colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_ResizeGripHovered] = blenderOrange;
            colors[ImGuiCol_ResizeGripActive] = blenderOrangeActive;

            // Tabs - Blender workspace tabs
            colors[ImGuiCol_Tab] = blenderVeryDarkGray;
            colors[ImGuiCol_TabHovered] = blenderMediumGray;
            colors[ImGuiCol_TabActive] = blenderDarkGray;                           // Active workspace
            colors[ImGuiCol_TabUnfocused] = blenderAlmostBlack;
            colors[ImGuiCol_TabUnfocusedActive] = blenderVeryDarkGray;

            // Docking - Blender's area system
            colors[ImGuiCol_DockingPreview] = ImVec4(0.98f, 0.55f, 0.18f, 0.60f);  // Orange preview
            colors[ImGuiCol_DockingEmptyBg] = blenderAlmostBlack;                   // Empty docking area

            // Plots - using Blender's graph editor colors
            colors[ImGuiCol_PlotLines] = blenderBlue;
            colors[ImGuiCol_PlotLinesHovered] = blenderBlueHover;
            colors[ImGuiCol_PlotHistogram] = blenderOrange;
            colors[ImGuiCol_PlotHistogramHovered] = blenderOrangeHover;

            // Text - Blender's text hierarchy
            colors[ImGuiCol_Text] = blenderWhite;                                   // Main text
            colors[ImGuiCol_TextDisabled] = blenderDarkText;                        // Disabled text
            colors[ImGuiCol_TextSelectedBg] = ImVec4(0.98f, 0.55f, 0.18f, 0.35f);  // Orange selection

            // Input fields - Blender number inputs and text fields
            colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);         // #1F1F1F - Dark inputs
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);  // Hover state
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);   // Active/focused

            // Modal backgrounds - Blender modal dialogs
            colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);

            // Tables - Blender outliner style
            colors[ImGuiCol_TableHeaderBg] = blenderVeryDarkGray;
            colors[ImGuiCol_TableBorderStrong] = blenderBorder;
            colors[ImGuiCol_TableBorderLight] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
            colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);      // Transparent
            colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);   // Subtle alternating

            // Drag and drop - Blender orange highlight
            colors[ImGuiCol_DragDropTarget] = blenderOrange;

            // Navigation - using orange theme
            colors[ImGuiCol_NavHighlight] = blenderOrange;
            colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
            colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

            // Style settings - Blender's UI characteristics
            style.WindowPadding = ImVec2(4.0f, 4.0f);                              // Tight padding like Blender
            style.WindowRounding = 10.0f;                                           // Sharp corners
            style.WindowBorderSize = 1.0f;                                         // Thin borders
            style.WindowMinSize = ImVec2(32.0f, 32.0f);
            style.WindowTitleAlign = ImVec2(0.5f, 0.5f);                          // Centered titles like Blender
            style.WindowMenuButtonPosition = ImGuiDir_Right;

            // Child windows
            style.ChildRounding = 8.0f;                                            // Sharp corners
            style.ChildBorderSize = 1.0f;

            // Popup windows
            style.PopupRounding = 4.0f;                                            // Slight rounding for popups
            style.PopupBorderSize = 1.0f;

            // Frame styling - Blender input field style
            style.FramePadding = ImVec2(6.0f, 3.0f);                              // Compact like Blender
            style.FrameRounding = 2.0f;                                           // Subtle rounding
            style.FrameBorderSize = 0.0f;                                         // No frame borders

            // Item spacing - tight like Blender interface
            style.ItemSpacing = ImVec2(4.0f, 2.0f);                               // Very tight
            style.ItemInnerSpacing = ImVec2(4.0f, 3.0f);
            style.CellPadding = ImVec2(4.0f, 2.0f);

            // Touch extra padding
            style.TouchExtraPadding = ImVec2(0.0f, 0.0f);

            // Indent - Blender outliner style
            style.IndentSpacing = 12.0f;                                          // Narrow indents
            style.ColumnsMinSpacing = 4.0f;

            // Scrollbar - thin like Blender
            style.ScrollbarSize = 12.0f;                                          // Thin scrollbars
            style.ScrollbarRounding = 0.0f;                                       // Sharp corners

            // Grab elements
            style.GrabMinSize = 8.0f;                                             // Small grabs
            style.GrabRounding = 2.0f;                                           // Slight rounding

            // Log slider
            style.LogSliderDeadzone = 4.0f;

            // Tabs - flat like Blender workspace tabs
            style.TabRounding = 10.0f;                                            // Sharp corners
            style.TabBorderSize = 0.0f;                                          // No borders
            style.TabMinWidthForCloseButton = 0.0f;

            // Tables
            style.TableAngledHeadersAngle = 35.0f * 3.142f / 180.0f;

            // Color button
            style.ColorButtonPosition = ImGuiDir_Right;

            // Button text alignment
            style.ButtonTextAlign = ImVec2(0.5f, 0.5f);                         // Centered
            style.SelectableTextAlign = ImVec2(0.0f, 0.5f);                     // Left-aligned selectables

            // Safe area padding
            style.DisplaySafeAreaPadding = ImVec2(1.0f, 1.0f);                  // Minimal padding

            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.Fonts->Clear();

            // Load fonts at different sizes
            ImFontConfig config;
            config.OversampleH = 2;
            config.OversampleV = 2;
            config.RasterizerMultiply = 1.25f;
            // Main UI font
            io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Open Sans/OpenSans-Regular.ttf", 18.0f, &config);
            Fonts::light = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Open Sans/OpenSans-Light.ttf", 18.0f, &config);
            Fonts::regular = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Open Sans/OpenSans-Bold.ttf", 18.0f, &config);

            // Code/monospace font for logs and values
            ImFontConfig monoConfig;
            monoConfig.OversampleH = 1;
            monoConfig.OversampleV = 1;
           // io.Fonts->AddFontFromFileTTF("P:/Projects/VS/zoe/iris/assets/fonts/Roboto-Regular.ttf", 16.0f, &monoConfig);

            // Build font atlas
            io.Fonts->Build();
        }
    }
}
#include "Editor.h"
#include "Style.h"

namespace Iaonnis
{

    float a = 0;
    float b[3]{};

    Iaonnis::Editor::Editor(GLFWwindow* window, std::shared_ptr<Scene> activeScene)
        :window(window), scene(activeScene)
	{
        glfwMakeContextCurrent(window);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;     
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;        
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;      

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        const char* glsl_version = "#version 130";

        ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
        ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
        ImGui_ImplOpenGL3_Init("#version 130");

        {
            style.FrameRounding = 5.0f;
            style.WindowRounding = 10.0f;

            style.ItemSpacing.x = 4.0f;
            style.ItemSpacing.y = 5.0f;

            style.ItemInnerSpacing = ImVec2(1.5f, 1.5f);


            //io.Fonts->AddFontFromFileTTF("Assets/Fonts/Montserrat-Bold.ttf", 18.0f);
            //io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto/Roboto_Condensed-Medium.ttf", 20.0f);

            ImVec4* colors = style.Colors;
            colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.11f, 1.0f);
            colors[ImGuiCol_Header] = ImVec4(0.22f, 0.29f, 0.38f, 1.0f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.33f, 0.40f, 0.55f, 1.0f);
            colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.37f, 0.37f, 0.38f, 1.0f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.19f, 0.47f, 0.26f, 1.0f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.25f, 1.0f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.30f, 0.35f, 1.0f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.45f, 0.25f, 1.0f);
            colors[ImGuiCol_Border] = ImVec4(0.3, 0.3, 0.3, 0.3);

        }

        SetBlenderTheme();
        InitializeDefaultPanels();
    }

    void Iaonnis::Editor::OnUpdate(Renderer3D::RendererStatistics stats, uint32_t r)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        renderOut = r;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static bool dockspaceOpen = true;
        static bool opt_fullscreen = true;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::Begin("##DockSpace", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar(2);

        ImGuiIO& io = ImGui::GetIO();
        auto& style = ImGui::GetStyle();
        float originalWindowMinX = style.WindowMinSize.x;
        style.WindowMinSize.x = 420.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        style.WindowMinSize.x = originalWindowMinX;

        ImGui::Begin("Debug");

        ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextAlign, ImVec2(0.5, 0.5));
        ImGui::SeparatorText("Frame Stats");
        ImGui::Text("FrameRate: %.1f FPS", io.Framerate);
        ImGui::Text("Delta Time: %.3f s", io.DeltaTime);

        ImGui::SeparatorText("Rendering");
        ImGui::Text("Draw Calls: %d", stats.nDrawCalls);
        ImGui::Text("Vertices: %d", stats.nRenderedVertices);
        ImGui::Text("Indices: %d", stats.nRenderedIndices);

        ImGui::SeparatorText("Memory");
        ImGui::Text("Textures: %.3f MB", (float)stats.totalTextureBufferSize / (1024.0f * 1024.0f));

        ImGui::SeparatorText("Upload Times");
        ImGui::Text("Scene Upload: %.3f ms", stats.sceneUploadTime);
        ImGui::Text("Material Upload: %.3f ms", stats.materialUploadTime);

        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

        ImGui::PopStyleVar(1);
        ImGui::End();

        menubar->OnUpdate();

        float dt = io.DeltaTime;

        for (auto& panel : panels)
        {
            panel->OnUpdate(dt);
        }


        ImGui::End(); //##DockSpace
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        io = ImGui::GetIO(); (void)io;
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void Editor::ShutDown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Editor::SelectEntt(Entity* entt)
    {
        SelectionData.selectedEntt = entt;
    }

    bool Editor::isEnttSelectionMade() 
    {
        if (SelectionData.selectedEntt == nullptr)
            return false;
        return true;
    }

    void Editor::DeselectAll()
    {
        SelectEntt(nullptr);
        SetSelectionIndex(-1);
        SetSelectionType(SelectionType::None);
    }

    void Editor::CreatePopUp(Event& event)
    {
        
    }

    void Editor::CreateScene()
    {
        scene.reset();
        scene = std::make_shared<Scene>("Scene");
    }

    void Editor::InitializeDefaultPanels()
    {
        menubar = std::make_unique<MenuBar>(this);
        panels.emplace_back(std::make_unique<SceneHierachy>(this));
        panels.emplace_back(std::make_unique<Inspector>(this));
        panels.emplace_back(std::make_unique<ViewPort>(this));
    }

}

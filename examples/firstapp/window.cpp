#include "window.hpp"

void Window::onCreate() {
  auto const &windowSettings{getWindowSettings()};
  fmt::print("Initial window size: {}x{}\n", windowSettings.width,
             windowSettings.height);
}

void Window::onPaint() {
  // Set the clear color
  abcg::glClearColor(m_clearColor.at(0), m_clearColor.at(1), m_clearColor.at(2),
                     m_clearColor.at(3));
  // Clear the color buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

static void ShowExampleMenuFile();

// Note that shortcuts are currently provided for display only
// (future version will add explicit flags to BeginMenu() to request processing shortcuts)
static void ShowExampleMenuFile()
{
    ImGui::MenuItem("(demo menu)", NULL, false, false);
    if (ImGui::MenuItem("New")) {}
    if (ImGui::MenuItem("Open", "Ctrl+O")) {}
    if (ImGui::BeginMenu("Open Recent"))
    {
        ImGui::MenuItem("fish_hat.c");
        ImGui::MenuItem("fish_hat.inl");
        ImGui::MenuItem("fish_hat.h");
        if (ImGui::BeginMenu("More.."))
        {
            ImGui::MenuItem("Hello");
            ImGui::MenuItem("Sailor");
            if (ImGui::BeginMenu("Recurse.."))
            {
                ShowExampleMenuFile();
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    if (ImGui::MenuItem("Save As..")) {}

    ImGui::Separator();
    if (ImGui::BeginMenu("Options"))
    {
        static bool enabled = true;
        ImGui::MenuItem("Enabled", "", &enabled);
        ImGui::BeginChild("child", ImVec2(0, 60), true);
        for (int i = 0; i < 10; i++)
            ImGui::Text("Scrolling Text %d", i);
        ImGui::EndChild();
        static float f = 0.5f;
        static int n = 0;
        ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
        ImGui::InputFloat("Input", &f, 0.1f);
        ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Colors"))
    {
        float sz = ImGui::GetTextLineHeight();
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
            ImGui::Dummy(ImVec2(sz, sz));
            ImGui::SameLine();
            ImGui::MenuItem(name);
        }
        ImGui::EndMenu();
    }

    // Here we demonstrate appending again to the "Options" menu (which we already created above)
    // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
    // In a real code-base using it would make senses to use this feature from very different code locations.
    if (ImGui::BeginMenu("Options")) // <-- Append!
    {
        static bool b = true;
        ImGui::Checkbox("SomeOption", &b);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (ImGui::MenuItem("Checked", NULL, true)) {}
    if (ImGui::MenuItem("Quit", "Alt+F4")) {}
}

void Window::onPaintUI() {
  // Parent class will show fullscreen button and FPS meter
  abcg::OpenGLWindow::onPaintUI();
    static bool show_app_main_menu_bar = false;
    static bool show_app_documents = false;

    static bool show_app_console = false;
    static bool show_app_log = false;
    static bool show_app_layout = false;
    static bool show_app_property_editor = false;
    static bool show_app_long_text = false;
    static bool show_app_auto_resize = false;
    static bool show_app_constrained_resize = false;
    static bool show_app_simple_overlay = false;
    static bool show_app_fullscreen = false;
    static bool show_app_window_titles = false;
    static bool show_app_custom_rendering = false;

    static bool show_app_metrics = false;
    static bool show_app_stack_tool = false;
    static bool show_app_style_editor = false;
    static bool show_app_about = false;

  auto flags1{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize};
  // Our own ImGui widgets go below
  {
    // Window begin
    ImGui::Begin("Hello, First App!", nullptr, flags1);

    bool save{};
    static bool showCompliment{}; // Hold state

    // Menu Bar
    if (ImGui::BeginMenuBar()) {
      // File menu
      if (ImGui::BeginMenu("File")) {
        ImGui::MenuItem("Save", nullptr, &save);
        ImGui::EndMenu();
      }
      // View menu
      if (ImGui::BeginMenu("View")) {
        ImGui::MenuItem("Show Compliment", nullptr, &showCompliment);
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }

    if (save) {
      // Save file...
    }

    if (showCompliment) {
      ImGui::Text("You're a beautiful person.");
    }

    // Static text
    auto const &windowSettings{getWindowSettings()};
    ImGui::Text("Current window size: %dx%d (in windowed mode)",
                windowSettings.width, windowSettings.height);

    // Slider from 0.0f to 1.0f
    static float f{};
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

    // ColorEdit to change the clear color
    ImGui::ColorEdit3("clear color", m_clearColor.data());

    // More static text
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // 100x50 button
    if (ImGui::Button("Press me!", ImVec2(100, 50))) {
      fmt::print("1st button pressed.\n");
    }

    // Nx50 button, where N is the remaining width available
    ImGui::Button("Press me!", ImVec2(-1, 50));
    // See also IsItemHovered, IsItemActive, etc
    if (ImGui::IsItemClicked()) {
      fmt::print("2nd Button pressed.\n");
    }

    static bool enabled{true};
    ImGui::Checkbox("Some option", &enabled);
    // fmt::print("The checkbox is {}\n", enabled ? "enabled" : "disabled");

    static std::size_t currentIndex{};
    std::vector comboItems{"AAA", "BBB", "CCC"};

    if (ImGui::BeginCombo("Combo box", comboItems.at(currentIndex))) {
      for (auto index{0U}; index < comboItems.size(); ++index) {
        bool const isSelected{currentIndex == index};
        if (ImGui::Selectable(comboItems.at(index), isSelected))
          currentIndex = index;

        // Set the initial focus when opening the combo (scrolling + keyboard
        // navigation focus)
        if (isSelected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }

    ImGui::SetNextWindowSize(ImVec2(300, 100));
    auto flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize};
    ImGui::Begin("Window with menu", nullptr, flags);
    {

      // Menu Bar
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Menu")) {
          ShowExampleMenuFile();
          ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Examples")) {
          ImGui::MenuItem("Main menu bar", NULL, &show_app_main_menu_bar);
          ImGui::MenuItem("Console", NULL, &show_app_console);
          ImGui::MenuItem("Log", NULL, &show_app_log);
          ImGui::MenuItem("Simple layout", NULL, &show_app_layout);
          ImGui::MenuItem("Property editor", NULL, &show_app_property_editor);
          ImGui::MenuItem("Long text display", NULL, &show_app_long_text);
          ImGui::MenuItem("Auto-resizing window", NULL, &show_app_auto_resize);
          ImGui::MenuItem("Constrained-resizing window", NULL,
                          &show_app_constrained_resize);
          ImGui::MenuItem("Simple overlay", NULL, &show_app_simple_overlay);
          ImGui::MenuItem("Fullscreen window", NULL, &show_app_fullscreen);
          ImGui::MenuItem("Manipulating window titles", NULL,
                          &show_app_window_titles);
          ImGui::MenuItem("Custom rendering", NULL, &show_app_custom_rendering);
          ImGui::MenuItem("Documents", NULL, &show_app_documents);
          ImGui::EndMenu();
        }
        // if (ImGui::MenuItem("MenuItem")) {} // You can also use MenuItem()
        // inside a menu bar!
        if (ImGui::BeginMenu("Tools")) {
          ImGui::MenuItem("Metrics/Debugger", NULL, &show_app_metrics);
          ImGui::MenuItem("Stack Tool", NULL, &show_app_stack_tool);
          ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);
          ImGui::MenuItem("About Dear ImGui", NULL, &show_app_about);
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }
    }
    ImGui::End();

    static std::array pos2d{0.0f, 0.0f};
    ImGui::SliderFloat2("2D position", pos2d.data(), 0.0, 50.0);

    static std::array pos3d{0.0f, 0.0f, 0.0f};
    ImGui::SliderFloat3("3D position", pos3d.data(), -1.0, 1.0);

    // fmt::print("Selected combo box item: {}\n", comboItems.at(currentIndex));

    // Window end
    ImGui::End();
  }
}


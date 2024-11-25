#include "window.h"
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>
#include <iostream>
#include "../input/keyboard.h"
#include "../input/mouse.h"
#include "../graphics/image.h"
#include "../../external/imgui/imgui.h"
#include "../../external/imgui/backends/imgui_impl_glfw.h"
#include "../../external/imgui/backends/imgui_impl_opengl3.h"
#include <set>
#include <map>
#include <algorithm>
#include <unordered_map>

void SetupImGuiStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);

    style.WindowRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.TabRounding = 0.0f;
}

int windowWidth, windowHeight;
bool WINDOW_INITIALIZED = false;

#pragma region Keyboard

namespace KeyboardState
{
    std::array<bool, 512> current;
    std::array<bool, 512> previous;
}

bool Keyboard::isDown(Key key)
{
    return KeyboardState::current[static_cast<int>(key)];
}

bool Keyboard::isPressed(Key key)
{
    return KeyboardState::current[static_cast<int>(key)] && !KeyboardState::previous[static_cast<int>(key)];
}

bool Keyboard::isReleased(Key key)
{
    return !KeyboardState::current[static_cast<int>(key)] && KeyboardState::previous[static_cast<int>(key)];
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE)
    {
        KeyboardState::current[key] = false;
    }
    else if (action == GLFW_PRESS)
    {
        KeyboardState::current[key] = true;
    }
}

#pragma endregion

#pragma region Mouse

namespace MouseState
{
    std::array<bool, 3> current;
    std::array<bool, 3> previous;
    double x, y;
    double xPrev, yPrev;
    double xScroll, yScroll;
}

bool Mouse::isDown(MouseButton button)
{
    return MouseState::current[static_cast<int>(button)];
}

bool Mouse::isPressed(MouseButton button)
{
    return MouseState::current[static_cast<int>(button)] && !MouseState::previous[static_cast<int>(button)];
}

bool Mouse::isReleased(MouseButton button)
{
    return !MouseState::current[static_cast<int>(button)] && MouseState::previous[static_cast<int>(button)];
}

double Mouse::getX()
{
    return MouseState::x;
}

double Mouse::getY()
{
    return MouseState::y;
}

int Mouse::getXScroll()
{
    return MouseState::xScroll;
}

int Mouse::getYScroll()
{
    return MouseState::yScroll;
}

double Mouse::getXDelta()
{
    return MouseState::x - MouseState::xPrev;
}

double Mouse::getYDelta()
{
    return MouseState::y - MouseState::yPrev;
}

static void mousePositionCallback(GLFWwindow* window, double x, double y)
{
    MouseState::x = x;
    MouseState::y = y;
}

static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    MouseState::xScroll = xoffset;
    MouseState::yScroll = yoffset;
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_RELEASE)
    {
        MouseState::current[button] = false;
    }
    else if (action == GLFW_PRESS)
    {
        MouseState::current[button] = true;
    }
}

void Window::toggleMouse()
{
    // Toggle mouse
    m_mouseActive = !m_mouseActive;

    // Set mouse to active or inactive based on what the new mouse state is
    glfwSetInputMode(
        m_window,
        GLFW_CURSOR,
        m_mouseActive ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED
    );

    // Set cursor position to center of screen
    glfwSetCursorPos(m_window, windowWidth / 2.0, windowHeight / 2.0);
}

void Window::disableMouse()
{
    m_mouseActive = true;
    toggleMouse();
}

void Window::enableMouse()
{
    m_mouseActive = false;
    toggleMouse();
}

bool Window::isMouseEnabled() const
{
    return m_mouseActive;
}

void Window::toggleDebugMenu()
{
    if (isMouseEnabled())
    {
        disableMouse();
        m_debugWindow = false;
    }
    else
    {
        enableMouse();
        m_debugWindow = true;
    }
}

std::string Window::getCaption()
{
    return glfwGetWindowTitle(m_window);
}

void Window::setCaption(const std::string& caption)
{
    glfwSetWindowTitle(m_window, caption.c_str());
}

void Window::setIcon(const Image& image)
{
    GLFWimage i = { image.getWidth(), image.getHeight(), image.getData() };
    glfwSetWindowIcon(m_window, 1, &i);
}

#pragma endregion

void Window::beginDrawing()
{
    for (int i = 0; i < KeyboardState::current.size(); ++i) { KeyboardState::previous[i] = KeyboardState::current[i]; }
    for (int i = 0; i < MouseState::current.size(); ++i) { MouseState::previous[i] = MouseState::current[i]; }

    MouseState::xPrev = MouseState::x;
    MouseState::yPrev = MouseState::y;

    MouseState::xScroll = 0;
    MouseState::yScroll = 0;

    glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

static void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    windowWidth = width;
    windowHeight = height;
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
}

bool Window::open(const std::string& title, int winWidth, int winHeight)
{
    windowWidth = winWidth; windowHeight = winHeight;

    if (!WINDOW_INITIALIZED)
    {
        WINDOW_INITIALIZED = glfwInit();
    }

    if (!WINDOW_INITIALIZED)
    {
        return false;
    }

    glfwWindowHint(GLFW_RESIZABLE, true);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

    m_window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);
    if (m_window == nullptr)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return false;
    }

    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetScrollCallback(m_window, mouseScrollCallback);
    glfwSetCursorPosCallback(m_window, mousePositionCallback);
    glfwSetWindowSizeCallback(m_window, windowSizeCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();
    SetupImGuiStyle();

    if (!ImGui_ImplGlfw_InitForOpenGL(m_window, true) ||
        !ImGui_ImplOpenGL3_Init())
    {
        std::cout << "[WARNING] Could not initiate ImGui!\n";
    }
    else
    {
        std::cout << "[INFO] ImGui successfully initiated\n";
    }
}

void Window::close()
{
    glfwTerminate();
}

void Window::setSize(int w, int h)
{
    glfwSetWindowSize(m_window, w, h);
}

void Window::clearBackground(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

void Window::clearDepthBuffer()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

struct ContentBrowser
{
    struct FileEntry
    {
        std::string name;
        bool isDirectory;
        uintmax_t size;
        FileSystem::FileTimeType lastModified;
        std::string extension;

        FileEntry(const FileSystem::DirectoryEntry& entry) :
            name(entry.path().filename().string()),
            isDirectory(FileSystem::isDirectory(entry.path())),
            extension(isDirectory ? "" : entry.path().extension().string()),
            size(FileSystem::size(entry.path())),
            lastModified(entry.last_write_time()) {}
    };

    FileSystem::Path currentPath;
    std::vector<FileEntry> entries;
    char searchBuffer[256] = "";
    std::vector<FileSystem::Path> pathHistory;
    int historyIndex = -1;
    bool showHidden = false;

    enum class SortColumn {
        Name,
        DateModified,
        Type,
        Size
    };

    void navigateTo(const FileSystem::Path& newPath) {
        // Remove forward history if we're navigating from middle
        if (historyIndex < pathHistory.size() - 1) {
            pathHistory.erase(pathHistory.begin() + historyIndex + 1, pathHistory.end());
        }

        currentPath = newPath;
        pathHistory.push_back(newPath);
        historyIndex = pathHistory.size() - 1;
        refreshDirectory();
    }

    void drawBreadcrumbs()
    {
        // Back/Forward buttons
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

        // At the top with other constants:
        const ImVec4 ACTIVE_COLOR = ImVec4(0.2f, 0.4f, 0.8f, 1.0f);   // Blue
        const ImVec4 INACTIVE_COLOR = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);  // Gray

        // Replace the navigation buttons code:
        ImGui::PushStyleColor(ImGuiCol_Button, historyIndex > 0 ? ACTIVE_COLOR : INACTIVE_COLOR);
        if (ImGui::ArrowButton("<-", ImGuiDir_Left) && historyIndex > 0)
        {
            historyIndex--;
            currentPath = pathHistory[historyIndex];
            refreshDirectory();
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, historyIndex < pathHistory.size() - 1 ? ACTIVE_COLOR : INACTIVE_COLOR);
        if (ImGui::ArrowButton("->", ImGuiDir_Right) && historyIndex < pathHistory.size() - 1)
        {
            historyIndex++;
            currentPath = pathHistory[historyIndex];
            refreshDirectory();
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();

        bool hasParent = currentPath.has_parent_path() && (currentPath.parent_path() != currentPath);
        ImGui::PushStyleColor(ImGuiCol_Button, hasParent ? ACTIVE_COLOR : INACTIVE_COLOR);
        if (ImGui::ArrowButton("^", ImGuiDir_Up) && hasParent)
        {
            navigateTo(currentPath.parent_path());
        }
        ImGui::SameLine();
        ImGui::PopStyleColor();

        ImGui::PopStyleColor();

        // Root drive
        std::string rootName = currentPath.root_name().string() + currentPath.root_directory().string();
        if (ImGui::Button(rootName.c_str()))
        {
            navigateTo(currentPath.root_path());
        }

        // Build path parts
        std::vector<FileSystem::Path> parts;
        FileSystem::Path current = currentPath;
        while (current != current.root_path() && current.has_parent_path())
        {
            parts.push_back(current);
            current = current.parent_path();
        }

        // Display breadcrumbs
        for (int i = parts.size() - 1; i >= 0; --i)
        {
            auto& part = parts[i];
            std::string name = part.filename().string();

            if (i > 0)
            {
                name += " >";
            }

            ImGui::SameLine(0, 0);
            if (ImGui::Button(name.c_str()))
            {
                navigateTo(part);
            }
        }
    }

    SortColumn sortColumn = SortColumn::Name;
    bool sortAscending = true;

    ContentBrowser() {
        currentPath = FileSystem::getCurrentPath();
        pathHistory.push_back(currentPath);
        historyIndex = 0;
        refreshDirectory();
    }

    void refreshDirectory()
    {
        entries.clear();
        for (const auto& entry : FileSystem::DirectoryIterator(currentPath))
        {
            if (!showHidden)
            {
                const auto& filename = entry.path().filename().string();
                if (filename[0] == '.' || filename[0] == '$') continue;
            }
            entries.emplace_back(entry);
        }
        sortEntries();
    }

    void sortEntries()
    {
        auto compareEntries = [this](const FileEntry& a, const FileEntry& b)
        {
            // Always sort directories first
            if (a.isDirectory != b.isDirectory)
            {
                return a.isDirectory;
            }

            int comp = 0;
            switch (sortColumn)
            {
            case SortColumn::Name:
                comp = _stricmp(a.name.c_str(), b.name.c_str());
                break;
            case SortColumn::DateModified:
                comp = (a.lastModified < b.lastModified) ? -1 : 1;
                break;
            case SortColumn::Type:
                comp = _stricmp(a.extension.c_str(), b.extension.c_str());
                if (comp == 0) comp = _stricmp(a.name.c_str(), b.name.c_str());
                break;
            case SortColumn::Size:
                comp = (a.size < b.size) ? -1 : 1;
                if (comp == 0) comp = _stricmp(a.name.c_str(), b.name.c_str());
                break;
            }
            return sortAscending ? (comp < 0) : (comp > 0);
        };

        std::sort(entries.begin(), entries.end(), compareEntries);
    }

    std::string FormatFileSize(uintmax_t size)
    {
        const char* units[] = { "B", "KB", "MB", "GB", "TB" };
        int unitIndex = 0;
        double doubleSize = static_cast<double>(size);

        while (doubleSize >= 1024.0 && unitIndex < 4)
        {
            doubleSize /= 1024.0;
            unitIndex++;
        }

        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.1f %s", doubleSize, units[unitIndex]);
        return std::string(buffer);
    }

    std::string formatDateTime(FileSystem::FileTimeType time)
    {
        return "Please fix!";
        /*
        // Convert to system time
        auto systime = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(systime);

        // Format using safe localtime
        struct tm timeInfo;
        localtime_s(&timeInfo, &time_t);

        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", &timeInfo);
        return std::string(buffer);
        */
    }

    void Draw(std::string title = "")
    {
        ImGui::Begin(title.empty() ? "File Browser" : title.c_str(), nullptr, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Working Directory", NULL))
                {
                    if (auto workingDir = FileSystem::getCurrentPath(); workingDir != currentPath)
                    {
                        navigateTo(workingDir);
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Hidden Items", NULL, &showHidden))
                {
                    refreshDirectory();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Navigation
        drawBreadcrumbs();
        {
            float windowWidth = ImGui::GetWindowWidth();

            int searchoffset = windowWidth - 200.0f - 10.0f - ImGui::CalcTextSize("Search").x - 5.0f;
            ImGui::SetCursorPosX(searchoffset);

            float origY = ImGui::GetCursorPosY();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);
            ImGui::Text("Search");
            ImGui::SameLine(windowWidth - 200.0f - 10.0f);
            ImGui::SetCursorPosY(origY);
            ImGui::PushItemWidth(200.0f);
            ImGui::InputText("##search", searchBuffer, sizeof(searchBuffer));
            ImGui::PopItemWidth();
        }

        // Table
        constexpr ImGuiTableFlags flags =
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_Reorderable |
            ImGuiTableFlags_Hideable |
            ImGuiTableFlags_Sortable |
            ImGuiTableFlags_SortMulti |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_BordersOuter |
            ImGuiTableFlags_BordersV |
            ImGuiTableFlags_ScrollY;

        if (ImGui::BeginTable("files", 4, flags))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort);
            ImGui::TableSetupColumn("Date Modified", ImGuiTableColumnFlags_DefaultSort);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_DefaultSort);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_DefaultSort);
            ImGui::TableHeadersRow();

            ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs();
            if (sorts_specs && sorts_specs->SpecsDirty)
            {
                sortColumn = static_cast<SortColumn>(sorts_specs->Specs[0].ColumnIndex);
                sortAscending = sorts_specs->Specs[0].SortDirection == ImGuiSortDirection_Ascending;
                sortEntries();
                sorts_specs->SpecsDirty = false;
            }

            // Parent directory entry
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::Selectable("..", false, ImGuiSelectableFlags_SpanAllColumns))
            {
                if (currentPath.has_parent_path() && (currentPath.parent_path() != currentPath))
                {
                    navigateTo(currentPath.parent_path());
                }
            }

            // Directory contents
            for (const auto& entry : entries)
            {
                const std::string& name = entry.name;

                // Skip if doesn't match search
                if (strlen(searchBuffer) > 0 &&
                    name.find(searchBuffer) == std::string::npos)
                {
                    continue;
                }

                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                std::string displayName = name + ((entry.isDirectory) ? "/" : "");
                if (ImGui::Selectable(displayName.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                {
                    if (entry.isDirectory)
                    {
                        navigateTo(currentPath / name);
                    }
                }

                /*
                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Delete"))
                    {
                        fs::remove(currentPath / name);
                        refreshDirectory();
                    }
                    ImGui::EndPopup();
                }
                */

                ImGui::TableNextColumn();
                ImGui::Text(formatDateTime(entry.lastModified).c_str());

                ImGui::TableNextColumn();
                if (entry.isDirectory)
                {
                    ImGui::Text("Folder");
                }
                else if (!entry.extension.empty())
                {
                    std::string ext = entry.extension + " File";
                    ImGui::Text(ext.c_str());
                }

                ImGui::TableNextColumn();
                ImGui::Text(entry.isDirectory ? "" : FormatFileSize(entry.size).c_str());
            }
            ImGui::EndTable();
        }

        ImGui::End();
    }
};

ContentBrowser browser;
void Window::endDrawing()
{
    if (m_debugWindow)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Create"))
            {
                ImGui::OpenPopup("Create New");
            }
            if (ImGui::MenuItem("Open"))
            {
                // Handle open
            }
            if (ImGui::MenuItem("Save"))
            {
                // Handle save
            }
            if (ImGui::MenuItem("Save as.."))
            {
                // Handle save as
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginPopup("Create New"))
        {
            static char newFileName[256] = "";
            ImGui::InputText("Name", newFileName, sizeof(newFileName));

            if (ImGui::Button("Create File"))
            {
                std::ofstream(browser.currentPath / newFileName).close();
                browser.refreshDirectory();
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::Button("Create Folder"))
            {
                fs::create_directory(browser.currentPath / newFileName);
                browser.refreshDirectory();
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        browser.Draw();

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    glfwSwapBuffers(m_window);
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

int Window::getHeight() const
{
    return windowHeight;
}

int Window::getWidth() const
{
    return windowWidth;
}
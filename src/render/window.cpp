#include "window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>
#include <iostream>
#include "../input/keyboard.h"
#include "../input/mouse.h"
#include "../graphics/image.h"

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

void Window::endDrawing()
{
    for (int i = 0; i < KeyboardState::current.size(); ++i) { KeyboardState::previous[i] = KeyboardState::current[i]; }
    for (int i = 0; i < MouseState::current.size(); ++i) { MouseState::previous[i] = MouseState::current[i]; }

    MouseState::xPrev = MouseState::x;
    MouseState::yPrev = MouseState::y;

    MouseState::xScroll = 0;
    MouseState::yScroll = 0;

    glfwSwapBuffers(m_window);
    glfwPollEvents();
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
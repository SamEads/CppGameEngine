#pragma once

#include <string>
#include "../graphics/image.h"

typedef struct GLFWwindow GLFWwindow;

class Window
{
private:
    GLFWwindow* m_window = nullptr;

    bool m_mouseActive = true;

public:
    Window() = default;

    ~Window();

    bool open(const std::string& title, int windowWidth, int windowHeight);

    void beginDrawing();

    void close();

    void setSize(int w, int h);

    void clearBackground(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    void clearDepthBuffer();

    void endDrawing();

    bool shouldClose() const;

    int getHeight() const;

    int getWidth() const;

    void toggleMouse();

    void disableMouse();

    void enableMouse();

    bool isMouseEnabled() const;

    std::string getCaption();

    void setCaption(const std::string& caption);

    void setIcon(const Image& image);
};
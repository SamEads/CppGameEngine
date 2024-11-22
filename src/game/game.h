#pragma once

#include <string>
#include "../memory/pointers.h"
#include "../utility/timer.h"
#include "../utility/vec.h"
#include "../sound/soundmanager.h"
#include <thread>
#include <mutex>
#include "../render/window.h"

class State;

class Game
{
protected:
    virtual void draw();

    virtual void tick();

    virtual void update();

    virtual void onScreenResize();

private:
    Window m_window;
    Timer m_timer;

    ScopedPtr<State> m_state;
    ScopedPtr<SoundManager> m_soundManager;
    
    Vec2<int> m_screenSize { 0 };
    
    bool m_running = false;

    std::thread renderThread;
    std::thread tickThread;

    std::mutex timerMutex;
    std::mutex logicMutex;
    std::mutex renderMutex;

    void preUpdate();

    void preDraw();

    void postDraw();

    void setState(ScopedPtr<State> state);

public:
    Game(const std::string& windowCaption, int tps, int startWidth, int startHeight);

    template <typename StateType>
    Game& setState()
    {
        setState(MakeScoped<StateType>(this));
        return *this;
    }

    const float getAlpha()
    {
        const std::lock_guard<std::mutex> lock(timerMutex);

        return m_timer.getAlpha();
    }

    void setWindowSize(int width, int height);

    void setWindowCaption(const std::string& caption);

    void run();

    const Vec2<int> getWindowSize() const;

    Window* getWindow();
};
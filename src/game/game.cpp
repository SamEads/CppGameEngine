#include "game.h"
#include "state.h"
#include "../input/keyboard.h"
#include "../input/mouse.h"
#include "../utility/random.h"
#include "../../external/sol.hpp"
#include "../io/filesystem.h"

/*
class Tile
{
public:
    Tile(const std::string& name, int hardness, bool isTransparent)
        : m_name(name), m_hardness(hardness), m_isTransparent(isTransparent) {}

    std::string getName() const { return m_name; }
    void setName(const std::string& newName) { m_name = newName; }

    int getHardness() const { return m_hardness; }
    void setHardness(int newHardness) { m_hardness = newHardness; }

    bool getIsTransparent() const { return m_isTransparent; }
    void setIsTransparent(bool transparency) { m_isTransparent = transparency; }

private:
    std::string m_name;
    int m_hardness;
    bool m_isTransparent;
};
*/

Game::Game(const std::string& windowCaption, int tps, int startWidth, int startHeight) : m_timer(tps)
{
    /*
    sol::state state;
    state.open_libraries(sol::lib::base, sol::lib::package);

    state.new_usertype<Tile>("Tile",
        sol::constructors<Tile(const std::string&, int, bool)>(),
        "get_name", &Tile::getName,
        "set_name", &Tile::setName,
        "get_hardness", &Tile::getHardness,
        "set_hardness", &Tile::setHardness,
        "get_transparent", &Tile::getIsTransparent,
        "set_transparent", &Tile::setIsTransparent
        // , "test_field", &Tile::testField
    );

    state.script(R"(
function print_tile(tile)
    print("Tile Name: " .. tile:get_name())
    print("Tile Hardness: " .. tile:get_hardness())
    print("Is Transparent: " .. tostring(tile:get_transparent()))
    print()
end

-- create cobblestone
tile_cobblestone = Tile.new("Cobblestone", 3, false)
print_tile(tile_cobblestone)
print(tile_cobblestone.test_field)
)");

    Tile* cobblestone = state["tile_cobblestone"];
    std::cout << "Accessing cobblestone made from lua from C++:" << std::endl;
    std::cout << "Tile Name: " << cobblestone->getName() << std::endl;
    std::cout << "Tile Hardness: " << cobblestone->getHardness() << std::endl;
    std::cout << "Is Transparent: " << std::boolalpha << cobblestone->getIsTransparent() << std::endl;
    */

    m_screenSize = { startWidth, startHeight };

    // SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
    // SetTraceLogLevel(LOG_ERROR | LOG_INFO | LOG_WARNING);

    m_window.open("Minecraft", m_screenSize.x, m_screenSize.y);
    // InitWindow(startWidth, startHeight, windowCaption.c_str());

    m_soundManager = MakeScoped<SoundManager>();
}

void Game::onScreenResize()
{

}

void Game::setState(ScopedPtr<State> state)
{
    const std::lock_guard<std::mutex> lock(logicMutex);
    m_state = std::move(state);
}

void Game::update()
{

}

void Game::preDraw()
{
    m_window.beginDrawing();

    m_window.clearBackground(230, 205, 255, 255);
}

void Game::postDraw()
{
    m_window.endDrawing();
}

void Game::draw()
{
    if (Keyboard::isPressed(Key::SPACE))
    {
        m_soundManager->playSoundFX("random.click", 1.0f, 1.0f);
    }

    const std::lock_guard<std::mutex> lock(logicMutex);
    if (m_state)
    {
        m_state->draw(getAlpha());
    }
}

void Game::tick()
{
    std::cout << "[INFO] Tick thread started\n";

    while (m_running)
    {
        if (m_state == nullptr)
        {
            continue;
        }

        int tickCount = 0;

        // Update timer safely to get the amount of ticks to run
        {
            const std::lock_guard<std::mutex> lock(timerMutex);

            m_timer.update();

            tickCount = m_timer.getTickCount();
        }

        // See if a tick needs to be performed
        for (int i = 0; i < tickCount; ++i)
        {
            const std::lock_guard<std::mutex> lock(logicMutex);
            m_state->tick();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::cout << "[INFO] Tick thread joined\n";
}

void Game::setWindowSize(int width, int height)
{
    m_screenSize = { width, height };
    m_window.setSize(width, height);
}

void Game::setWindowCaption(const std::string& caption)
{
    m_window.setCaption(caption);
}

void Game::run()
{
    if (m_running)
    {
        std::cout << "[WARNING] Game::run was called while the game was already running. Only call once!\n";
        return;
    }

    m_running = true;
    tickThread = std::thread(&Game::tick, this);

    while (!m_window.shouldClose())
    {
        preUpdate();
        update();
        preDraw();
        draw();
        postDraw();
    }

    m_running = false;

    tickThread.join();

    m_window.close();
}

const Vec2<int> Game::getWindowSize() const
{
    return m_screenSize;
}

Window* Game::getWindow()
{
    return &m_window;
}

void Game::preUpdate()
{
    //updateControllers();

    //updateMouse({ GetMouseDelta().x, GetMouseDelta().y });

    m_soundManager->update();

    Vec2<float> screenSize = { (float)m_window.getWidth(), (float)m_window.getHeight() };

    if (screenSize != m_screenSize)
    {
        m_screenSize = screenSize;
        onScreenResize();
    }
}
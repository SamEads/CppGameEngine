#include <game/game.h>
#include <game/state.h>
#include <render/camera3d.h>

class Level : public State
{
private:
    Camera3D m_camera;

public:
    // Inherit state constructor
    Level(Game* game) : State(game)
    {
        
    }

    // Called once-per-tick. (fixed update - 60 TPS in this example)
    void tick() override
    {

    }

    // Called once-per-game frame.
    void draw(const float alpha) override
    {
        m_camera.beginDrawing({ 640, 480 });

        m_camera.endDrawing();
    }
};

int main()
{
    Game game("3D Scene Test", 60, 640, 480);
    game.setState<Level>().run();
}
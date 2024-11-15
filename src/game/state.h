#pragma once

class Game;

class State
{
protected:
    Game* m_game;

public:
    State(Game* game);
    State& operator =(const State& s) = delete;
    
    virtual void tick() {}
    virtual void draw(const float alpha) {}
    
    virtual ~State() = default;
};
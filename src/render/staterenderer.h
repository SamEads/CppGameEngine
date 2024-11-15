#pragma once

class State;

class StateRenderer
{
public:
    StateRenderer();
    virtual void draw(State* state, float alpha);
    virtual ~StateRenderer() = default;
};
#pragma once

#include <stack>
#include <mat4x4.hpp>

class MatrixStack
{
private:
    std::stack<glm::mat4> stack;

public:
    MatrixStack();

    void push();

    void pop();

    void loadIdentity()
    {
        stack.top() = glm::mat4(1.0f);
    }

    void translate(float x, float y, float z);

    void rotate(float angle, float x, float y, float z);

    void scale(float x, float y, float z);

    void insert(const glm::mat4& m);

    glm::mat4 top() const;
};
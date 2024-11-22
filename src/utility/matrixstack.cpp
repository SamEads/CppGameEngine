#include "matrixstack.h"
#include <ext/matrix_transform.hpp>

MatrixStack::MatrixStack()
{
    stack.push(glm::mat4(1.0f));
}

void MatrixStack::push()
{
    stack.push(stack.top());
}

void MatrixStack::pop()
{
    // can't pop more from stack than pushed by user
    // This keeps the identity matrix in tact
    if (stack.size() <= 1)
    {
        return;
    }
    stack.pop();
}

void MatrixStack::translate(float x, float y, float z)
{
    glm::vec3 v = { x, y, z };
    stack.top() = glm::translate(stack.top(), v);
}

void MatrixStack::rotate(float angle, float x, float y, float z)
{
    glm::vec3 axis = { x, y, z };
    stack.top() = glm::rotate(stack.top(), glm::radians(angle), axis);
}

void MatrixStack::scale(float x, float y, float z)
{
    glm::vec3 v = { x, y, z };
    stack.top() = glm::scale(stack.top(), v);
}

void MatrixStack::insert(const glm::mat4& m)
{
    stack.top() = m;
}

glm::mat4 MatrixStack::top() const
{
    return stack.top();
}
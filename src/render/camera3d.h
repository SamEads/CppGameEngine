#pragma once

#include "../memory/pointers.h"
#include "../utility/vec.h"
#include "../utility/mat.h"
#include "../utility/matrixstack.h"

class Window;

class Camera3D
{
private:
	float m_fov;
	int m_proj;
	int m_matrixPushes;

	glm::mat4 m_projM;
	glm::mat4 m_viewM;
	MatrixStack m_modelM;

	Window* m_window;

public:
	Camera3D(Window* window, float fov, int perspective = 0);

	void beginDrawing(const Vec2<int> screenSize, float farPlane);

	void endDrawing();

	float getFOV() const;

	void setFOV(float fov);

	void pushMatrix();

	void popMatrix();

	void orient(const float yaw, const float pitch);

	void rotate(float angle, float x, float y, float z);

	void translate(float x, float y, float z);

	void scale(float x, float y, float z);

	const glm::mat4& getView() const;

	MatrixStack* getModelStack();

	const glm::mat4& getProjection() const;

	const glm::mat4& getModel() const;
};
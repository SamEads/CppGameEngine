#pragma once

#include "../memory/pointers.h"
#include "../utility/vec.h"

typedef struct Camera3D;

class Cam3D
{
private:
	Vec3<float> m_position;
	Vec3<float> m_target;
	Vec3<float> m_up;
	float m_fov;
	int m_proj;
	int m_matrixPushes;

public:
	Cam3D(float fov, int perspective = 0);

	void beginDrawing(const Vec2<int> screenSize, float farPlane);

	void endDrawing();

	float getFOV() const;

	void setFOV(float fov);

	Vec3<float> getDirection() const;
	
	void pushMatrix();

	void popMatrix();

	void orient(const float yaw, const float pitch);

	void rotate(float angle, float x, float y, float z);

	void translate(float x, float y, float z);

	void scale(float x, float y, float z);

	void clearDepthBuffer();
};
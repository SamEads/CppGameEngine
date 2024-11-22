#pragma once

enum class MouseButton
{
	LEFT = 0,
	RIGHT = 1,
	MIDDLE = 2
};

namespace Mouse
{
	bool isDown(MouseButton button);
	bool isPressed(MouseButton button);
	bool isReleased(MouseButton button);
	double getX();
	double getY();
	int getXScroll();
	int getYScroll();
	double getXDelta();
	double getYDelta();
};
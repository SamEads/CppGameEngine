#pragma once
#include <string>
#include "../utility/vec.h"

typedef struct ALCdevice ALCdevice;
typedef struct ALCcontext ALCcontext;

class Sound3D;

class SoundManager
{
private:
	ALCdevice* device = nullptr;
	ALCcontext* context = nullptr;

	float m_volume = 1.0f;
	bool m_hasSound = false;

	bool validateSoundPath(const std::string& path) const;

	void playSound(std::unique_ptr<Sound3D> sound, float volume, float pitch) const;

public:
	SoundManager();

	void playSound(const std::string& path, float x, float y, float z, float volume, float pitch, float maxDistance = 16.0f) const;
	void playSoundFX(const std::string& path, float volume, float pitch) const;
	
	void updatePosition(Vec3<double> prevPos, Vec3<double> curPos, float prevYaw, float curYaw, const float alpha);
	
	void update();
	
	void setVolume(float volume);
	float getVolume() const;
};
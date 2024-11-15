#include <al.h>
#include <alc.h>
#include <alext.h>

extern "C"
{
	#include "../../external/stb_vorbis.h"
}

#include <vector>
#include <unordered_map>

#include "soundmanager.h"
#include "../utility/interpolate.h"
#include "../utility/defines.h"
#include "../utility/random.h"
#include "../io/filesystem.h"
#include "../memory/pointers.h"

class SoundBuffer
{
private:
	ALuint m_buffer = -1;
public:
	std::string path;
	void generateBuffer(std::string path)
	{
		this->path = path;

		int channels, sampleRate;
		short* output;
		int samples = stb_vorbis_decode_filename(path.c_str(), &channels, &sampleRate, &output);

		alGenBuffers(1, &m_buffer);
		alBufferData(m_buffer, channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, output, samples * sizeof(short) * channels, sampleRate);

		free(output);
	}
	ALuint& getBuffer() { return m_buffer; }
	~SoundBuffer()
	{
		alDeleteBuffers(1, &m_buffer);
	}
};

std::unordered_map<std::string, ScopedPtr<SoundBuffer>> soundBuffers;
class Sound3D
{
protected:
	ALuint m_source;
	SoundBuffer* m_buffer;
	virtual void setSoundParameters(float x, float y, float z, float maxDistance)
	{
		alSource3f(m_source, AL_POSITION, x, y, z);
		alSource3f(m_source, AL_VELOCITY, 0, 0, 0);
		alSourcef(m_source, AL_MAX_DISTANCE, maxDistance);
	}
public:
	Sound3D(std::string path, float x, float y, float z, float pitch, float maxDistance)
	{
		auto it = soundBuffers.find(path);
		if (it != soundBuffers.end())
		{
			m_buffer = it->second.get();
		}
		else
		{
			std::unique_ptr<SoundBuffer> bufPointer = MakeScoped<SoundBuffer>();
			bufPointer->generateBuffer(path);
			auto pair = soundBuffers.emplace(path, std::move(bufPointer));
			m_buffer = pair.first->second.get();
		}

		alGenSources(1, &m_source);

		alSourcef(m_source, AL_PITCH, pitch);
		alSourcei(m_source, AL_LOOPING, AL_FALSE);
		alSourcei(m_source, AL_SOURCE_RESAMPLER_SOFT, 1);
		setSoundParameters(x, y, z, maxDistance);
	}
	void play()
	{
		alSourcei(m_source, AL_BUFFER, m_buffer->getBuffer());
		alSourcePlay(m_source);
	}
	bool isPlaying()
	{
		ALint state;
		alGetSourcei(m_source, AL_SOURCE_STATE, &state);
		return state == AL_PLAYING;
	}
	~Sound3D()
	{
		alDeleteSources(1, &m_source);
	}
	void setVolume(float volume)
	{
		alSourcef(m_source, AL_GAIN, volume);
	}
};

class Sound2D : public Sound3D
{
protected:
	void setSoundParameters(float x, float y, float z, float maxDistance) override
	{
		alSource3f(m_source, AL_POSITION, x, y, z);
		alSource3f(m_source, AL_VELOCITY, 0, 0, 0);
		alSourcei(m_source, AL_SOURCE_RELATIVE, AL_TRUE);
	}
public:
	Sound2D(std::string path, float pitch, float maxDistance) : Sound3D(path, 0, 0, 0, pitch, maxDistance) {};
};

class SoundPool
{
private:
	Random m_random;
	std::unordered_map<std::string, std::vector<std::string>> soundPaths;
	/*
	Gather all sounds and add them to a combined list for each sound.
	Format of a file as EX. would be:
		- "sound/step/grass[1,2,3,4].ogg"	->	"step.grass" (calling card, note omission of numbers)
		- "sound/liquid/lava.ogg"			->	"liquid.lava" (calling card)
	*/
	void populateSoundPaths(const FileSystem::Path& directory)
	{
		if (!FileSystem::exists(directory))
		{
			return;
		}

		for (const auto& entry : FileSystem::RecursiveDirectoryIterator(directory))
		{
			if (FileSystem::isRegularFile(entry.path()))
			{
				std::string fullPath = entry.path().string();
				std::string clippedPath = fullPath.substr(directory.string().length() + 1);
				size_t lastDot = clippedPath.find_last_of(".");
				if (lastDot != std::string::npos)
					clippedPath = clippedPath.substr(0, lastDot);

				// Clip number out if applicable to get path of vector
				size_t lastDigit = clippedPath.find_last_not_of("0123456789");
				if (lastDigit != std::string::npos)
					clippedPath = clippedPath.substr(0, lastDigit + 1);

				// RegEx if it wasn't fucking around:
				std::replace(clippedPath.begin(), clippedPath.end(), '/', '.');
				std::replace(clippedPath.begin(), clippedPath.end(), '\\', '.');

				// Add to entries
				soundPaths[clippedPath].push_back(fullPath);
			}
		}
	}
public:
	SoundPool()
	{
		populateSoundPaths("assets/sound");
	}
	// Returns full path of sound
	std::string getSound(const std::string& clippedPath)
	{
		auto it = soundPaths.find(clippedPath);

		// No sound, return equivalent to null string
		if (it == soundPaths.end())
			return "";

		const auto& paths = it->second;

		// Fixed index, can return
		if (paths.size() == 1)
			return paths[0];

		// Get random index
		int index = m_random.nextInt(paths.size());
		return paths[index];
	}
};

SoundPool soundPool;
std::vector<std::unique_ptr<Sound3D>> sounds;

SoundManager::SoundManager()
{
	device = alcOpenDevice(NULL);

	m_hasSound = (device) ? true : false;

	if (m_hasSound)
	{
		context = alcCreateContext(device, NULL);
		if (!context)
		{
			m_hasSound = false;
		}
		else
		{
			alcMakeContextCurrent(context);
		}
	}

	if (m_hasSound)
	{
		alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	}
	else
	{
		std::cout << "[WARNING] Sound system was not initiated. There will be no sound for this session\n";
	}
}

bool SoundManager::validateSoundPath(const std::string& path) const
{
	return
		m_volume > 0.0f
		&& m_hasSound
		&& !path.empty();
}

void SoundManager::playSound(std::unique_ptr<Sound3D> sound, float volume, float pitch) const
{
	Sound3D* soundPtr = sound.get();
	sounds.emplace_back(std::move(sound));

	soundPtr->setVolume(m_volume * (volume * 0.25f));
	soundPtr->play();
}

void SoundManager::playSound(const std::string& path, float x, float y, float z, float volume, float pitch, float maxDistance) const
{
	if (!validateSoundPath(path))
	{
		return;
	}

	playSound(MakeScoped<Sound3D>(soundPool.getSound(path), x, y, z, pitch, maxDistance), volume, pitch);
}

void SoundManager::playSoundFX(const std::string& path, float volume, float pitch) const
{
	if (!validateSoundPath(path))
	{
		return;
	}

	playSound(MakeScoped<Sound2D>(soundPool.getSound(path), pitch, 16.0f), volume, pitch);
}

void SoundManager::update()
{
	for (auto it = sounds.begin(); it != sounds.end();)
	{
		auto& soundAt = *it;
		if (!soundAt->isPlaying())
		{
			it = sounds.erase(it);
		}
		else ++it;
	}
}

void SoundManager::setVolume(float volume)
{
	m_volume = volume;
}

float SoundManager::getVolume() const
{
	return m_volume;
}

void SoundManager::updatePosition(Vec3<double> prevPos, Vec3<double> curPos, float prevYaw, float curYaw, const float alpha)
{
	float yaw = interpolate(prevYaw, curYaw, alpha);

	// when viewing the player from the front, the audio needs to be panned the other way
	/*
	if (perspective == 2)
	{
		yaw += 180.f;
	}
	*/

	Vec3<double> interp = interpolate(prevPos, curPos, alpha);
	float cYaw = cos(-yaw * (PI / 180.f) - PI);
	float sYaw = sin(-yaw * (PI / 180.f) - PI);

	float dirX = -sYaw;
	float dirY = 0.f;
	float dirZ = -cYaw;

	// direction & up vector
	alListener3f(AL_POSITION, static_cast<ALfloat>(interp.x), static_cast<ALfloat>(interp.y), static_cast<ALfloat>(interp.z));
	ALfloat listenerOri[] = { dirX, dirY, dirZ, 0.f, 1.f, 0.f };
	alListenerfv(AL_ORIENTATION, listenerOri);
}
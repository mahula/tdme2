// Generated from /tdme/src/tdme/audio/Audio.java

#pragma once

#include <map>
#include <string>

#include <fwd-tdme.h>
#include <tdme/audio/AudioBufferManager.h>
#include <tdme/audio/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>

using std::map;
using std::wstring;

using tdme::audio::AudioBufferManager;
using tdme::audio::AudioEntity;
using tdme::math::Vector3;

/** 
 * Interface to TDME audio methods
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::audio::Audio final
{
public: /* protected */
	static constexpr int32_t ALBUFFERID_NONE { -1 };
	static constexpr int32_t ALSOURCEID_NONE { -1 };
	static Audio* instance;

private:
	map<wstring, AudioEntity*> audioEntities;

public: /* protected */
	AudioBufferManager audioBufferManager {  };
	Vector3 listenerPosition {  };
	Vector3 listenerVelocity {  };
	Vector3 listenerOrientationAt {  };
	Vector3 listenerOrientationUp {  };

public:
	/** 
	 * @return audio singleton instance
	 */
	static Audio* getInstance();

public:

	/** 
	 * @return listener position
	 */
	Vector3& getListenerPosition();

	/** 
	 * @return listener velocity
	 */
	Vector3& getListenerVelocity();

	/** 
	 * @return listener orientation at
	 */
	Vector3& getListenerOrientationAt();

	/** 
	 * @return listener orientation up
	 */
	Vector3& getListenerOrientationUp();

	/** 
	 * Returns an audio entity identified by given id
	 * @param id
	 * @return audio entity
	 */
	AudioEntity* getEntity(const wstring& id);

	/** 
	 * Adds an stream
	 * the only format supported by now is ogg vorbis
	 * @param id
	 * @param path name
	 * @param file name
	 * @return audio entity
	 */
	AudioEntity* addStream(const wstring& id, const wstring& pathName, const wstring& fileName);

	/** 
	 * Adds an sound
	 * the only format supported by now is ogg vorbis
	 * @param id
	 * @param path name
	 * @param file name
	 * @return audio entity
	 */
	AudioEntity* addSound(const wstring& id, const wstring& pathName, const wstring& fileName);

	/** 
	 * Removes an audio entity
	 * @param id
	 */
	void removeEntity(const wstring& id);

	/** 
	 * Clears all audio entities
	 */
	void reset();

	/** 
	 * Shuts the audio down
	 */
	void shutdown();

	/** 
	 * Update and transfer audio entity states to open AL
	 */
	void update();

	// Generated

private:
	/**
	 * Private constructor
	 */
	Audio();
};

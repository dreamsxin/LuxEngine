#pragma once


#include "engine/iplugin.h"


namespace Lumix
{


class AudioSystem;
class Clip;
class Path;


class AudioScene : public IScene
{
public:
	typedef int SoundHandle;
	static const SoundHandle INVALID_SOUND_HANDLE = -1;

	struct ClipInfo
	{
		Clip* clip;
		char name[30];
		uint32 name_hash;
		bool looped;
	};

public:
	static AudioScene* createInstance(AudioSystem& system,
		Universe& universe,
		class IAllocator& allocator);
	static void destroyInstance(AudioScene* scene);

	virtual int getClipCount() const = 0;
	virtual ClipInfo* getClipInfo(int index) = 0;
	virtual ClipInfo* getClipInfo(const char* name) = 0;
	virtual int getClipInfoIndex(ClipInfo* info) = 0;
	virtual void addClip(const char* name, const Lumix::Path& path) = 0;
	virtual void removeClip(ClipInfo* clip) = 0;
	virtual void setClip(int clip_id, const Lumix::Path& path) = 0;

	virtual ClipInfo* getAmbientSoundClip(ComponentIndex cmp) = 0;
	virtual void setAmbientSoundClip(ComponentIndex cmp, ClipInfo* clip) = 0;
	virtual bool isAmbientSound3D(ComponentIndex cmp) = 0;
	virtual void setAmbientSound3D(ComponentIndex cmp, bool is_3d) = 0;

	virtual SoundHandle play(Entity entity, ClipInfo* clip, bool is_3d) = 0;
	virtual void stop(SoundHandle sound_id) = 0;
	virtual void setVolume(SoundHandle sound_id, float volume) = 0;
};


} // namespace Lumix
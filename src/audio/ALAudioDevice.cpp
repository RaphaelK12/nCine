#include <cstdlib> // for exit()
#include "ALAudioDevice.h"
#include "AudioBufferPlayer.h"
#include "AudioStreamPlayer.h"
#include "ServiceLocator.h"

namespace ncine {

namespace {

void playPlayer(IAudioPlayer *player) { player->play(); }
void pausePlayer(IAudioPlayer *player) { player->pause(); }
void stopPlayer(IAudioPlayer *player) { player->stop(); }

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ALAudioDevice::ALAudioDevice()
	: device_(NULL), context_(NULL), gain_(1.0f)
{
	device_ = alcOpenDevice(NULL);
	if (device_ == NULL)
	{
		LOGF_X("alcOpenDevice failed: %x", alGetError());
		exit(EXIT_FAILURE);
	}

	context_ = alcCreateContext(device_, NULL);
	if (context_ == NULL)
	{
		alcCloseDevice(device_);
		LOGF_X("alcCreateContext failed: %x", alGetError());
		exit(EXIT_FAILURE);
	}

	if (!alcMakeContextCurrent(context_))
	{
		alcDestroyContext(context_);
		alcCloseDevice(device_);
		LOGF_X("alcMakeContextCurrent failed: %x", alGetError());
		exit(EXIT_FAILURE);
	}

	alGenSources(MaxSources, sources_.data());
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerf(AL_GAIN, gain_);
}

ALAudioDevice::~ALAudioDevice()
{
	for (unsigned int i = 0; i < MaxSources; i++)
	{
		alSourcei(sources_[i], AL_BUFFER, AL_NONE);
	}
	alDeleteSources(MaxSources, sources_.data());

	alcDestroyContext(context_);

	ALCboolean result = alcCloseDevice(device_);
	if (!result)
	{
		LOGF_X("alcCloseDevice failed: %d", alGetError());
		exit(EXIT_FAILURE);
	}
}


///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ALAudioDevice::setGain(ALfloat gain)
{
	gain_ = gain;
	alListenerf(AL_GAIN, gain_);
}

void ALAudioDevice::stopPlayers()
{
	forEach(players_.begin(), players_.end(), stopPlayer);
	players_.clear();
}

void ALAudioDevice::pausePlayers()
{
	forEach(players_.begin(), players_.end(), pausePlayer);
	players_.clear();
}

void ALAudioDevice::freezePlayers()
{
	forEach(players_.begin(), players_.end(), pausePlayer);
	// The player list is not cleared at this point, it is needed as-is by the unfreeze method
}

void ALAudioDevice::unfreezePlayers()
{
	forEach(players_.begin(), players_.end(), playPlayer);
}

int ALAudioDevice::nextAvailableSource()
{
	ALint sourceState;

	for (unsigned int i = 0; i < MaxSources; i++)
	{
		alGetSourcei(sources_[i], AL_SOURCE_STATE, &sourceState);
		if (sourceState != AL_PLAYING && sourceState != AL_PAUSED)
		{
			return sources_[i];
		}
	}

	return -1;
}

void ALAudioDevice::registerPlayer(IAudioPlayer *player)
{
	players_.pushBack(player);
}

void ALAudioDevice::updatePlayers()
{
	List<IAudioPlayer *>::ConstIterator i = players_.begin();
	while (i != players_.end())
	{
		if ((*i)->isPlaying())
		{
			(*i)->updateState();
			++i;
		}
		else
		{
			i = players_.erase(i);
		}
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Stops or pauses all buffer players
void ALAudioDevice::stopOrPauseBufferPlayers(bool shouldStop)
{
	List<IAudioPlayer *>::ConstIterator i = players_.begin();
	while (i != players_.end())
	{
		if ((*i)->type() == AudioBufferPlayer::sType())
		{
			if (shouldStop)
			{
				(*i)->stop();
			}
			else
			{
				(*i)->pause();
			}
			i = players_.erase(i);
		}
		else
		{
			++i;
		}
	}
}

/// Stops or pauses all stream players
void ALAudioDevice::stopOrPauseStreamPlayers(bool shouldStop)
{
	List<IAudioPlayer *>::ConstIterator i = players_.begin();
	while (i != players_.end())
	{
		if ((*i)->type() == AudioStreamPlayer::sType())
		{
			if (shouldStop)
			{
				(*i)->stop();
			}
			else
			{
				(*i)->pause();
			}
			i = players_.erase(i);
		}
		else
		{
			i++;
		}
	}
}

}

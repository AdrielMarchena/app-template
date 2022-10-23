#include "pch.h"
#include "SoundsSystemFMOD.h"

#include "Debug/Intrumentator.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include "FMODErrors.h"
namespace Game
{

static inline float ChangeOctave(float frequency, float variation)
{
	static const float octave_ratio = 2.0f;
	return frequency * std::pow(octave_ratio, variation);
}

static inline float ChangeSemiton(float frequency, float variation)
{
	static const float semitone_ratio = std::pow(2.0f, 1.0f / 12.0f);
	return frequency * std::pow(semitone_ratio, variation);
}

FMODSystem::~FMODSystem()
{
	if (SystemPtr)
		SystemPtr->release();
}

Ref<FMODChannel> FMODSound::GetChannel()
{
	GAME_PROFILE_FUNCTION();
	auto wp = SystemPtr.lock();
	GAME_WARN_IF_NOT(wp, "Tried to get channel for sound {0}, but the sound system already expired", SoundPath);
	FMOD::Channel* channel{ nullptr };
	auto error = wp->SystemPtr->playSound(SoundPtr, 0, true, &channel);
	FMODErrorCheckPath(error, SoundPath);
	return MakeRef<FMODChannel>(channel);
}

void FMODSound::HotPlay()
{
	GAME_PROFILE_FUNCTION();
	auto wp = SystemPtr.lock();
	GAME_WARN_IF_NOT(wp, "Tried to play {0}, but the sound system already expired", SoundPath);
	auto error = wp->SystemPtr->playSound(SoundPtr, 0, false, 0);
	FMODErrorCheckPath(error, SoundPath);
}

FMODSound::~FMODSound()
{
	if (!SystemPtr.expired())
		SoundPtr->release();
}

SoundsSystemFMOD::SoundsSystemFMOD()
{
	GAME_PROFILE_FUNCTION();
	m_System = MakeRef<FMODSystem>();
	auto error = FMOD::System_Create(&m_System->SystemPtr);
	FMODErrorCheck(error);

	m_System->SystemPtr->init(100, FMOD_INIT_NORMAL, 0);
}

void SoundsSystemFMOD::Update(float dt)
{
	m_System->SystemPtr->update();
}

Ref<FMODSound> SoundsSystemFMOD::CreateFMODSound(const std::string& filepath, FMODSound::SoundMode mode) const
{
	Ref<FMODSound> wrapSound = MakeRef<FMODSound>();
	wrapSound->Mode = mode;
	wrapSound->SoundPath = filepath;
	wrapSound->SystemPtr = MakeWeak(m_System);
	return wrapSound;
}

Ref<FMODSound> SoundsSystemFMOD::CreateSoundRef(const std::string& filepath, int32_t mode) const
{
	GAME_PROFILE_FUNCTION();
	auto wrapSound = CreateFMODSound(filepath, FMODSound::SoundMode::Sound);

	auto error = m_System->SystemPtr->createSound(filepath.c_str(), mode | FMOD_LOOP_NORMAL, 0, &wrapSound->SoundPtr);
	FMODErrorCheckPath(error, filepath);
	return wrapSound;
}

Ref<FMODSound> SoundsSystemFMOD::CreateStreamRef(const std::string& filepath, int32_t mode) const
{
	GAME_PROFILE_FUNCTION();
	auto wrapSound = CreateFMODSound(filepath, FMODSound::SoundMode::Stream);

	auto error = m_System->SystemPtr->createStream(filepath.c_str(), mode | FMOD_LOOP_NORMAL, 0, &wrapSound->SoundPtr);
	FMODErrorCheckPath(error, filepath);
	return wrapSound;
}

FMODChannel::~FMODChannel()
{
}

void FMODChannel::Play()
{
	bool playing = false;
	auto error = ChannelPtr->isPlaying(&playing);
	FMODErrorCheck(error);
	GAME_WARN_IF(playing, "The sound on this channel is already playing");
	SetPause(false);
}

void FMODChannel::Stop()
{
	auto error = ChannelPtr->stop();
	FMODErrorCheck(error);
}

void FMODChannel::SetPause(bool pause)
{
	auto error = ChannelPtr->setPaused(pause);
	FMODErrorCheck(error);
}

void FMODChannel::SetPosition(unsigned int ms)
{
	auto error = ChannelPtr->setPosition(ms, FMOD_TIMEUNIT_MS);
	FMODErrorCheck(error);
}

void FMODChannel::LoopEndless()
{
	auto error = ChannelPtr->setLoopCount(-1);
	FMODErrorCheck(error);
}

void FMODChannel::SetLoopCount(int loopCount)
{
	auto error = ChannelPtr->setLoopCount(loopCount);
	FMODErrorCheck(error);
}

void FMODChannel::Restart()
{
	auto error = ChannelPtr->setPosition(0, FMOD_TIMEUNIT_MS);
	FMODErrorCheck(error);
}

void FMODChannel::SetVolume(float volume)
{
	auto error = ChannelPtr->setVolume(volume);
	FMODErrorCheck(error);
}

void FMODChannel::Mute()
{
	auto error = ChannelPtr->setMute(true);
	FMODErrorCheck(error);
}

void FMODChannel::UnMute()
{
	auto error = ChannelPtr->setMute(false);
	FMODErrorCheck(error);
}

void FMODChannel::SetMute(bool mute)
{
	auto error = ChannelPtr->setMute(mute);
	FMODErrorCheck(error);
}

void FMODChannel::SetFrequency(float frequency)
{
	auto error = ChannelPtr->setFrequency(frequency);
	FMODErrorCheck(error);
}

float FMODChannel::GetFrequency() const
{
	float f = 0.0f;
	auto error = ChannelPtr->getFrequency(&f);
	FMODErrorCheck(error);
	return f;
}

void FMODChannel::UpOctave(float octaves)
{
	m_Frequency = GetFrequency();
	m_Frequency = ChangeOctave(m_Frequency, octaves);
	SetFrequency(m_Frequency);
}

void FMODChannel::DownOctave(float octaves)
{
	m_Frequency = GetFrequency();
	m_Frequency = ChangeOctave(m_Frequency, -octaves);
	SetFrequency(m_Frequency);
}

void FMODChannel::UpSemitone(float semitones)
{
	m_Frequency = GetFrequency();
	m_Frequency = ChangeSemiton(m_Frequency, semitones);
	SetFrequency(m_Frequency);
}

void FMODChannel::DownSemitone(float semitones)
{
	m_Frequency = GetFrequency();
	m_Frequency = ChangeSemiton(m_Frequency, -semitones);
	SetFrequency(m_Frequency);
}
}

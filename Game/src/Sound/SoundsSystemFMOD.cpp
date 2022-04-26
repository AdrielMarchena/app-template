#include "pch.h"
#include "SoundsSystemFMOD.h"

#include "Debug/Intrumentator.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include "FMODErrors.h"
namespace Game
{
    FMODSystem::~FMODSystem()
    {
        if(SystemPtr)
            SystemPtr->release();
    }

    Ref<FMODChannel> FMODSound::GetChannel()
    {
        GAME_PROFILE_FUNCTION();
        auto wp = SystemPtr.lock();
        GAME_WARN_IF_NOT(wp, "Tried to get channel for sound {0}, but the sound system already expired", SoundPath);
        FMOD::Channel* channel{nullptr};
        auto error = wp->SystemPtr->playSound(SoundPtr, 0, true, &channel);
        FMODErrorCheckPath(error, SoundPath);
        GAME_CORE_ASSERT(error == FMOD_OK, "FMOD ERROR | Something went wrong when playing {0}\n\tError: {1}", SoundPath, FMOD_ErrorString(error));
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

    Ref<FMODSound> SoundsSystemFMOD::CreateFMODSound(const std::string& filepath, FMODSound::SoundMode mode)
    {
        Ref<FMODSound> wrapSound = MakeRef<FMODSound>();
        wrapSound->Mode = mode;
        wrapSound->SoundPath = filepath;
        wrapSound->SystemPtr = MakeWeak(m_System);
        return wrapSound;
    }

    Ref<FMODSound> SoundsSystemFMOD::CreateSoundRef(const std::string& filepath, int32_t mode)
    {
        GAME_PROFILE_FUNCTION();
        auto wrapSound = CreateFMODSound(filepath, FMODSound::SoundMode::Sound);
        
        auto error = m_System->SystemPtr->createSound(filepath.c_str(), mode, 0, &wrapSound->SoundPtr);
        FMODErrorCheckPath(error,filepath);
        return wrapSound;
    }

    Ref<FMODSound> SoundsSystemFMOD::CreateStreamRef(const std::string& filepath, int32_t mode)
    {
        GAME_PROFILE_FUNCTION();
        auto wrapSound = CreateFMODSound(filepath, FMODSound::SoundMode::Stream);

        auto error = m_System->SystemPtr->createStream(filepath.c_str(), mode, 0, &wrapSound->SoundPtr);
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
        SetPause(true);
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

    void FMODChannel::Restart()
    {
        auto error = ChannelPtr->setPosition(0, FMOD_TIMEUNIT_MS);
        FMODErrorCheck(error);
    }
}

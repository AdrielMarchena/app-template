#include "pch.h"
#include "SoundsSystemFMOD.h"

#include "Debug/Intrumentator.h"
#include "fmod.hpp"
#include "fmod_errors.h"

namespace Game
{
    FMODSystem::~FMODSystem()
    {
        if(SystemPtr)
            SystemPtr->release();
    }

    void FMODSound::Play()
    {
        // TODO: Implement
        //SystemPtr->playSound()        
    }

    void FMODSound::HotPlay()
    {
        GAME_PROFILE_FUNCTION();
        auto wp = SystemPtr.lock();
        GAME_DEBUG_WARN_IF_NOT(wp, "Tried to play {0}, but the sound system already expired");
        auto error = wp->SystemPtr->playSound(SoundPtr, 0, false, 0);
        GAME_CORE_ASSERT(error == FMOD_OK, "FMOD ERROR | Something went wrong when playing {0}\n\tError: {1}", SoundPath , FMOD_ErrorString(error));
    }

    void FMODSound::Stop()
    {
        // TODO: Implement
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
        GAME_CORE_ASSERT(error == FMOD_OK, "FMOD ERROR | Something went wrong when creating sound system\n\tError: {0}", FMOD_ErrorString(error));

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
        GAME_CORE_ASSERT(error == FMOD_OK, "FMOD ERROR | Something went wrong when creating sound\n\tError: {0}", FMOD_ErrorString(error));
        return wrapSound;
    }

    Ref<FMODSound> SoundsSystemFMOD::CreateStreamRef(const std::string& filepath, int32_t mode)
    {
        GAME_PROFILE_FUNCTION();
        auto wrapSound = CreateFMODSound(filepath, FMODSound::SoundMode::Stream);

        auto error = m_System->SystemPtr->createStream(filepath.c_str(), mode, 0, &wrapSound->SoundPtr);
        GAME_CORE_ASSERT(error == FMOD_OK, "FMOD ERROR | Something went wrong when creating sound stream\n\tError: {0}", FMOD_ErrorString(error));
        return wrapSound;
    }
}

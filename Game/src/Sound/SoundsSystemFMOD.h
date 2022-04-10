#pragma once

#include "Core/Core.h"

namespace FMOD
{
    class System;
    class Sound;
}

namespace Game
{
    struct FMODSystem
    {
    public:
        ~FMODSystem();
    private:
        FMOD::System* SystemPtr = nullptr;
        friend class FMODSound;
        friend class SoundsSystemFMOD;
    };

    struct FMODSound
    {
    private:
        std::string SoundPath = "";
        FMOD::Sound* SoundPtr{nullptr};
        Weak<FMODSystem> SystemPtr; // Reference to system 
    public:
        enum class SoundMode
        {
            Sound,
            Stream
        } Mode;
        void Play();
        void HotPlay();
        void Stop();

        ~FMODSound();
    private:
        friend class SoundsSystemFMOD;
    };

    class SoundsSystemFMOD
    {
    private:
        Ref<FMODSystem> m_System;
    public:
        SoundsSystemFMOD();
        ~SoundsSystemFMOD() = default;
        SoundsSystemFMOD(const SoundsSystemFMOD&) = delete;
        SoundsSystemFMOD& operator=(const SoundsSystemFMOD&) = delete;

        void Update(float dt);

        // Wrap things in Ref
        Ref<FMODSound> CreateSoundRef(const std::string& filepath, int32_t mode);
        Ref<FMODSound> CreateStreamRef(const std::string& filepath, int32_t mode);

    private:
        Ref<FMODSound> CreateFMODSound(const std::string& filepath, FMODSound::SoundMode mode);
    };
}

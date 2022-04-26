#pragma once

#include "Core/Core.h"

namespace FMOD
{
    class System;
    class Sound;
    class Channel;
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

    struct FMODChannel
    {
    public:
        FMODChannel() = default;
        ~FMODChannel();
        void Play();
        /* It is not the same as pausing the sound, this release the channel for other sounds to use */
        void Stop();
        void SetPause(bool pause);
        void SetPosition(unsigned int ms);
        void Restart();
    private:
        FMODChannel(FMOD::Channel* channel)
            :ChannelPtr(channel) 
        {
            GAME_CORE_ASSERT(channel, "Channel parameter can't be nullptr");
        }
        FMOD::Channel* ChannelPtr = nullptr;
        friend class FMODSound;
        friend class FMODSystem;
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
        Ref<FMODChannel> GetChannel();
        void HotPlay();

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

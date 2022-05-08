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
    // Forward declarations
    struct FMODSound;
    struct FMODChannel;

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
        Ref<FMODChannel> GetChannel();
        void HotPlay();

        ~FMODSound();
    private:
        friend class SoundsSystemFMOD;
    };

    struct FMODChannel
    {
    private:
        float m_Frequency = 0.0f;
    public:
        FMODChannel() = default;
        FMODChannel(FMOD::Channel* channel)
            :ChannelPtr(channel)
        {
            GAME_CORE_ASSERT(channel, "Channel parameter can't be nullptr");
        }
        ~FMODChannel();
        void Play();
        /* It is not the same as pausing the sound, this release the channel for other sounds to use */
        void Stop();
        void SetPause(bool pause);
        void SetPosition(unsigned int ms);
        void LoopEndless();
        /* 0 is one time, 1 is two */
        void SetLoopCount(int loopCount);
        void Restart();
        void SetVolume(float volume);
        void Mute();
        void UnMute();
        void SetMute(bool mute);
        void SetFrequency(float frequency);
        float GetFrequency() const;

        void UpOctave(float octaves = 1.0f);
        void DownOctave(float octaves = 1.0f);

        void UpSemitone(float semitones = 1.0f);
        void DownSemitone(float semitones = 1.0f);
    private:
        FMOD::Channel* ChannelPtr = nullptr;
        Ref<FMODSound> m_Sound;
        friend class FMODSound;
        friend class FMODSystem;
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
        Ref<FMODSound> CreateSoundRef(const std::string& filepath, int32_t mode) const;
        Ref<FMODSound> CreateStreamRef(const std::string& filepath, int32_t mode) const;

    private:
        Ref<FMODSound> CreateFMODSound(const std::string& filepath, FMODSound::SoundMode mode) const;
    };
}

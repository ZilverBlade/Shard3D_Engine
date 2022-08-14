#pragma once
#include <miniaudio.h>
#include <stdio.h>
#include "../../core.h"
#include <glm/glm.hpp>
namespace Shard3D {
    struct AudioProperties {
        float volume{ 1.f };
        float pitch{ 0.f };
        glm::vec3 relativePos{ 0.f };
    };
    enum AudioParams {
        AudioNull,
        AudioSpatialize,
        AudioOneShot
    };

    class EngineAudio {
        static inline ma_result result;
        static inline ma_engine engine;

        ma_sound sound;
        typedef int audio_params_t;
    public:

        ~EngineAudio();

        static void init();
        static void destroy();
        static void globalUpdate(glm::vec3 position, glm::vec3 dir);

        void play(const std::string& file, audio_params_t params = 0);
        void stop();
        void update(AudioProperties& properties);
        void pause();
        void resume();
    };
}
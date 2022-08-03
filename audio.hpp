#pragma once
#include <miniaudio.h>
#include <stdio.h>
#include "engine_logger.hpp"
#include <glm/glm.hpp>
namespace Shard3D {
    class EngineAudio {
        _S3D_GVAR ma_result result;
        _S3D_GVAR ma_engine engine;

        ma_sound sound;
        typedef int audio_params_t;
    public:

        ~EngineAudio();
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
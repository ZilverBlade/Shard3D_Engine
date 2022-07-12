#pragma once
#include <miniaudio.h>
#include <stdio.h>
#include "engine_logger.hpp"
#include <glm/glm.hpp>
namespace Shard3D {
    class EngineAudio {
        inline static ma_result result;
        inline static ma_engine engine;

        ma_sound sound;

    public:

        ~EngineAudio();
        struct AudioProperties {
            float volume{ 1.f };
            float pitch{ 0.f };
            glm::vec3 relativePos{ 0.f };
        };

        static void init();
        static void destroy();
        static void globalUpdate(glm::vec3 position, glm::vec3 dir);

        void play(const std::string& file);
        void stop();
        void update(AudioProperties& properties);
        void pause();
        void resume();
    };
}
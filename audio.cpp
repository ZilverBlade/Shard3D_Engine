#include "s3dtpch.h"
#define MINIAUDIO_IMPLEMENTATION
#include "audio.hpp"
#include <iostream>
#include "singleton.hpp"
#include "wb3d/actor.hpp"

namespace Shard3D {
    EngineAudio::~EngineAudio() {}
    void EngineAudio::init() {
        result = ma_engine_init(NULL, &engine);
        if (result != MA_SUCCESS) {
            SHARD3D_ERROR("Failed to initialize audio engine! ({0})", result);
        }
    }
    void EngineAudio::destroy() {
        ma_engine_uninit(&engine);
    }

    void EngineAudio::play(const std::string& file) {
        result = ma_sound_init_from_file(&engine, file.c_str(), 0, NULL, NULL, &sound);
        if (result != MA_SUCCESS) {
            SHARD3D_ERROR("Failed to initialize sound '{0}'! ({1})", file.c_str(), result);
        }
        ma_sound_start(&sound);
        ma_sound_set_looping(&sound, true);
    }
    void EngineAudio::globalUpdate(glm::vec3 position, glm::vec3 dir) {
        ma_engine_listener_set_position(&engine, 0, position.x, position.y, position.z);
       
      
    }
    void EngineAudio::update(AudioProperties& properties) {
        ma_sound_set_pitch(&sound, properties.pitch);
        ma_sound_set_volume(&sound, properties.volume);
        ma_sound_set_position(&sound, properties.relativePos.x, properties.relativePos.y, properties.relativePos.z);
    }

    void EngineAudio::pause() {
        ma_sound_stop(&sound);
    }

    void EngineAudio::resume() {
        ma_sound_start(&sound);
    }

    void EngineAudio::stop() {
        ma_sound_stop(&sound);
        ma_sound_uninit(&sound);
        if (ma_sound_is_playing(&sound) == MA_TRUE)
            SHARD3D_ERROR("Failed to destroy sound '{0}'!", (void*)sound.pDataSource);
    }

}
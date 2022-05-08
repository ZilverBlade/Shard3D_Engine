#include "fmod_core_engine.hpp"
#include <cstddef>

#include <fmod.hpp>
#include <iostream>

#pragma warning(disable : 4996)

namespace fmod_engine {


    FMOD_RESULT result;
    FMOD::System* system = NULL;
    
    FMOD::Sound* sound0;
    FMOD::Channel* channel = 0;

        void FMOD_Core_Engine::PlaySoundFile(const char* soundPath) {
            result = FMOD::System_Create(&system);

            result = system->init(512, 0, 0);
            result = system->createSound(soundPath, FMOD_DEFAULT, 0, &sound0);
            result = system->playSound(sound0, 0, false, &channel);

            result = system->update();

            std::cout << "Playing sound file: " << soundPath << std::endl;
        }

        void FMOD_Core_Engine::Update(){
            result = system->update();
        }
        void FMOD_Core_Engine::UpdateVolume(float volume /*from 0 to 1*/) {
            result = channel->setVolume(volume);
            result = system->update();
        }
        void FMOD_Core_Engine::UpdatePitch(float pitch /*relative octaves (0.5 = half, 2 = double)*/) {
            result = channel->setPitch(pitch);
            result = system->update();
        }

        void FMOD_Core_Engine::Release() {
            result = system->release();
        }

}
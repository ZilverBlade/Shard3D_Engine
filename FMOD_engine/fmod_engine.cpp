#include "fmod_engine.hpp"
#include <cstddef>

#include <fmod.hpp>
namespace fmod_engine {

    FMOD_RESULT result;
    FMOD::System* system = NULL;
    FMOD::Sound* sound0;
    FMOD::Channel* channel = 0;

        void FMOD_Engine::PlaySound(const char* soundPath)
        {   
            result = FMOD::System_Create(&system);

            result = system->init(512, 0, 0);
            result = system->createSound(soundPath, FMOD_DEFAULT, 0, &sound0);
            result = system->playSound(sound0, 0, false, &channel);

            result = system->update();
        }

        void FMOD_Engine::Release()
        {
            result = system->release();
        }
    
}
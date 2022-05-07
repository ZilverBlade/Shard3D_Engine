#include "run_app.hpp"
#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include <fmod.hpp>
#include <string>




/*
    ShardEngine (2022 All rights reserved) created by ZilverBlade and Razer
    Huge shoutout to Brendan Galea https://www.youtube.com/channel/UC9pXmjxsQHeFH9vgCeRsHcw
    And also SaschaWillems' GitHub repository with examples https://github.com/SaschaWillems/Vulkan
*/

int main() {

    /*
    char filename[] = "libs/py/main.py"; //path the the file here
	FILE* fp;

	Py_Initialize();

	fp = _Py_fopen(filename, "r");
	PyRun_SimpleFile(fp, filename);

	Py_Finalize();
    
  */

    //move this code to a different executable/dll or move it to the fmod_engine.cpp file
    FMOD_RESULT result;
    FMOD::System* system = NULL;
    FMOD::Sound* sound1;
    FMOD::Channel* channel = 0;
    result = FMOD::System_Create(&system);

    result = system->init(512, 0, 0);

    result = system->createSound("sounddata/sigma.mp3", FMOD_DEFAULT, 0, &sound1);
    result = system->playSound(sound1, 0, false, &channel);

    result = system->update();

    shard::RunApp app{};
    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
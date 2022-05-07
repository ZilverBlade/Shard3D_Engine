#include "run_app.hpp"
#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include "FMOD_engine/fmod_engine.hpp"
#include <string>
//#include <Python.h>



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
	int var = static_cast<uint32_t>(PyRun_SimpleFile(fp, filename));
    std::cout << var << std::endl;
	Py_Finalize();
    */

    fmod_engine::FMOD_Engine fmodengine;
    fmodengine.PlaySound("sounddata/9kkkkkkkkkk.wav"); //zyn :)
    
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
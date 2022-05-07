#include "run_app.hpp"
#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include "FMOD_engine/fmod_engine.hpp"
#include <string>

/*
    ShardEngine (2022 All rights reserved) created by ZilverBlade and Razer
    Huge shoutout to Brendan Galea https://www.youtube.com/channel/UC9pXmjxsQHeFH9vgCeRsHcw
    And also SaschaWillems' GitHub repository with examples https://github.com/SaschaWillems/Vulkan
*/

int main() {
    int blob;   

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
    std::cin >> blob;
    return EXIT_SUCCESS;
}
#include "run_app.hpp"
#include "settings_initialiser.cpp"
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <Python.h>

/*
    ShardEngine (2022 All rights reserved) created by ZilverBlade and Razer
    Huge shoutout to Brendan Galea https://www.youtube.com/channel/UC9pXmjxsQHeFH9vgCeRsHcw
    And also SaschaWillems' GitHub repository with examples https://github.com/SaschaWillems/Vulkan
*/

int main() {


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
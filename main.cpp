#include "run_app.hpp"
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <string>

/*
    Shard3D (2022 All rights reserved) created by ZilverBlade and Razer
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
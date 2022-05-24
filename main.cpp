#include "run_app.hpp"
#include <iostream>
#include <cstdlib>
#include <stdexcept>

/*
    Shard3D 1.0 (2022) created by ZilverBlade and Razer
*/
int main() {

    Shard3D::RunApp app{};
    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

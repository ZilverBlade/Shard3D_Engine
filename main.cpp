#include "run_app.hpp"
#include <iostream>
#include <cstdlib>
#include <stdexcept>

/*
    Shard3D 1.0 (2022) created by ZilverBlade
*/
int main() {

    std::ifstream infile(ENGINE_SETTINGS_PATH);
    assert(infile.good() != false && "Critical error! Engine settings config file not found!");
    std::ifstream infile2(GAME_SETTINGS_PATH);
    assert(infile2.good() != false && "Critical error! Game settings config file not found!");

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

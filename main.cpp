#include "run_app.hpp"
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include "utils/dialogs.h"
/*
    Shard3D 1.0 (2022) created by ZilverBlade
*/
int main() {
    /*
    std::ifstream infile(ENGINE_SETTINGS_PATH);
    if (infile.good() == false) throw std::runtime_error("Critical error! Engine settings config file not found!");
    std::ifstream infile2(GAME_SETTINGS_PATH);
    if (infile2.good() == false) throw std::runtime_error("Critical error! Game settings config file not found!");
    */

    Shard3D::RunApp app{};

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        Shard3D::MessageDialogs::show(e.what(), "FATAL ERROR!!!", Shard3D::MessageDialogs::OPTICONERROR);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

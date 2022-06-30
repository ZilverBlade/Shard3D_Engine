#include "run_app.hpp"
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include "utils/dialogs.h"
#include "engine_logger.hpp"
#include "cheat_codes.hpp"
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
    Shard3D::LOGGER::init();
#ifdef _WIN32
    ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif
#ifndef NDEBUG
    SHARD3D_INFO("Validation layers enabled");
#endif
    Shard3D::RunApp app{};
#if BETA_DEBUG_TOOLS == false // dont hide for beta testing, as it might be useful
#ifdef GAME_RELEASE_READY  // hide for deploy, and logging would be done in a file anyway, console is only for dev purposes
#ifdef _WIN32 // this function is exclusive to windows
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
#endif
#endif
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

#include "s3dtpch.h" 
#include "editor_app.hpp"
#include "project.hpp"

#include "cheat_codes.hpp"
#include "graphics_settings.hpp"

/*
    Shard3D 1.0 (2022) created by ZilverBlade
*/
int main() {
#ifdef _WIN32
    ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif
#ifndef _DEPLOY
    Shard3D::EditorApp app{};
#endif
#ifdef _DEPLOY
    Shard3D::ProjectApp app{};
#endif
#if ENSET_BETA_DEBUG_TOOLS == false // dont hide for beta testing, as it might be useful
#ifdef _DEPLOY  // hide for deploy, and logging would be done in a file anyway, console is only for dev purposes
#ifdef _WIN32 // this function is exclusive to windows
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
#endif
#endif
    try {
        app.run();
    }
    catch (const std::exception& e) {
        SHARD3D_FATAL(e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

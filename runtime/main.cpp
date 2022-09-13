#include "application.h"
#include <Shard3D/utils/logger.h>
/*
    Shard3D 1.0 (2022) created by ZilverBlade
*/

void initEngineFeatures() {
    Shard3D::LOGGER::init();
}
#ifdef _DEPLOY
int invoke_main() {
#elif !_DEPLOY
int main(int argc, char* argv[], char* env[]) {
#endif
    initEngineFeatures();

#ifdef _WIN32
    ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif
    Shard3D::Application app{};
#ifndef ENSET_BETA_DEBUG_TOOLS // dont hide for beta testing, as it might be useful
#ifdef _DEPLOY  // hide for deploy, and logging would be done in a file anyway, console is only for dev purposes
#ifdef _WIN32 // this function is exclusive to windows
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
#endif
#endif

    SHARD3D_ASSERT(argc == 2 && "Too many / Too little arguments provided for runtime!");
    SHARD3D_INFO("Level file chosen: {0}", argv[1]);
    app.run(argv[1]);
    return EXIT_SUCCESS;
}

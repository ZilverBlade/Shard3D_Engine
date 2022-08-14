#include "application.h"
#include <Shard3D/utils/logger.h>
/*
    Shard3D 1.0 (2022) created by ZilverBlade
*/

void initEngineFeatures() {
    Shard3D::LOGGER::init();
}

int main() {
    initEngineFeatures();

#ifdef _WIN32
    ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif
    Shard3D::EngineApplication app{};
#ifndef ENSET_BETA_DEBUG_TOOLS // dont hide for beta testing, as it might be useful
#ifdef _DEPLOY  // hide for deploy, and logging would be done in a file anyway, console is only for dev purposes
#ifdef _WIN32 // this function is exclusive to windows
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
#endif
#endif
  //  try {
      app.run();
   // }
   // catch (const std::exception& e) {
   //     SHARD3D_FATAL(e.what());
   //     return EXIT_FAILURE;
   // }
    return EXIT_SUCCESS;
}

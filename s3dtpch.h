#pragma once
#define NOMINMAX
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_INCLUDE_VULKAN
//std
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <cassert>
#include <cstring>
#include <fstream>
#include <array>
#include <limits>
#include <sstream>
#include <typeinfo>
#include <cstdlib>

//extern
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

//project
#include "engine_logger.hpp"
#include "utils/dialogs.h"
#include "utils/engine_utils.hpp"
#include "utils/yaml_ext.hpp"
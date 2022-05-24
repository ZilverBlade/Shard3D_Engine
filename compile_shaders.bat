glslc.exe shaders/basic_shader.vert -o shaders/basic_shader.vert.spv
glslc.exe shaders/basic_shader.frag -o shaders/basic_shader.frag.spv
glslc.exe shaders/grid.vert -o shaders/grid.vert.spv
glslc.exe shaders/grid.frag -o shaders/grid.frag.spv


glslc.exe shaders/pointlight.vert -o shaders/pointlight.vert.spv
glslc.exe shaders/pointlight.frag -o shaders/pointlight.frag.spv
glslc.exe shaders/spotlight.vert -o shaders/spotlight.vert.spv
glslc.exe shaders/spotlight.frag -o shaders/spotlight.frag.spv
glslc.exe shaders/directional_light.vert -o shaders/directional_light.vert.spv
glslc.exe shaders/directional_light.frag -o shaders/directional_light.frag.spv

pause

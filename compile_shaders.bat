glslc.exe shaders/ez_shader.vert -o shaders/ez_shader.vert.spv
glslc.exe shaders/ez_shader.frag -o shaders/ez_shader.frag.spv

glslc.exe shaders/ez_shader.vert -o bin/Shard3D/x64/Release/shaders/ez_shader.vert.spv
glslc.exe shaders/ez_shader.frag -o bin/Shard3D/x64/Release/shaders/ez_shader.frag.spv
glslc.exe shaders/ez_shader.vert -o bin/Shard3D/x64/Debug/shaders/ez_shader.vert.spv
glslc.exe shaders/ez_shader.frag -o bin/Shard3D/x64/Debug/shaders/ez_shader.frag.spv

pause


![](https://cdn.discordapp.com/attachments/763044823342776340/1002705748121354302/unknown.png)  

# Shard3D

Shard3D Engine, a 2000s styled engine running on Vulkan 1.3

The goal is to get an partial editor based engine with 2000s styled graphics with modern technology, with tons of customisation.
Read more on the website: https://www.shard3d.com

### Requirements

Requires version **1.3.211.0** of Vulkan SDK: https://vulkan.lunarg.com/sdk/home#windows
Make sure to install GLM and SDL2, and that the vulkan environment variable states VULKAN_SDK.

**Make sure to compile with MSVC (VS2019) as other build systems are not supported at the moment** 

**Extra details about building:** 
Since this project now uses precompiled headers, and has multi-processor compiling enabled, 
make sure for the first build to switch 'Use (/Yu)' to 'Create (/Yc)', and then switch it back to 'Use (/Yu)' when it's done, to be able to use multi-processor compilation.

### Contributors

- [@ZilverBlade](https://www.github.com/ZilverBlade)
- [@DhiBaid](https://www.github.com/DHIBAID)


### Editor Camera Controls

- Look Around = Right MB
- Move Left = A
- Move Right = D
- Move Forward = W
- Move Backward = S
- Move Up = SPACE
- Move Down = RIGHT CONTROL
- Slow Down Movement = LEFT CONTROL
- Speed Up Movement = LEFT SHIFT
- Adjust FOV = Scroll Wheel

# What Shard3D offers

### Scripting

Shard3D supports scripting in both C# and Visual Basic, and you may use them interchangeably.
C++ Native scripting is also available, but does not have the same flexibility as previously mentioned languages, and must be hard-coded in the project.

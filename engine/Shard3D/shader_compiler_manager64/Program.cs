using System;
using System.IO;
using System.Diagnostics;

namespace shadercompmgr
{
    class Program {
        readonly static string compiler = "glslc.exe";
        static string inShaderLoc = "shaders/";
        static string outShaderLoc = "shaders/";

        static int totalCount = 0;
        static int vertCount = 0;
        static int fragCount = 0;
        static int geomCount = 0;

        static void Main(string[] args) {
            if (args.Length < 3) {
                if (args.Length == 1 && args[0] == "-h") {
                    Process.Start(compiler, $"-h");
                }
                Console.WriteLine("Insufficient arguments specified");
                Console.WriteLine("Usage: 'glslc.exe action' 'raw shader location' 'compiled shader location'\n");
                Console.WriteLine("glslc.exe action: type in shadercompmgr -h for more listings");
                Console.WriteLine("raw shader location: path");
                Console.WriteLine("compiled shader location: path");
                ThrownErr();
            }

            inShaderLoc = args[1];
            outShaderLoc = args[2];

            Console.WriteLine("ShaderCompMgr64 0.1");

            foreach (string shaderFile in Directory.GetFiles(inShaderLoc)) {
                if (shaderFile.EndsWith(".spv")) continue;
                totalCount += 1;

                // going off by regular standard of naming scemes, if the extension isnt one of these, it will assume it's some kind of other shader file, even if it's not
                if (shaderFile.EndsWith(".vert"))
                    vertCount+=1;
                else if (shaderFile.EndsWith(".frag"))
                    fragCount += 1;
                else if (shaderFile.EndsWith(".geom"))
                    geomCount += 1;
            }
            Console.WriteLine($"Total of {totalCount} shaders found ({vertCount} vertex shaders, " +
                $"{fragCount} fragment shaders, {geomCount} geometry shaders, {totalCount - vertCount - fragCount - geomCount} misc shaders)\n");

            CompileShaders();
        }

        static void CompileShaders() {
            int index = 0;
            foreach (string shaderFile in Directory.GetFiles(inShaderLoc))
            {
                if (shaderFile.EndsWith(".spv")) continue; // check if the file is not already a compiled file, otherwise it will attempt to compile a compiled shader
                index += 1;

                if (shaderFile.EndsWith(".vert"))
                    Console.WriteLine($"Compiling vertex shader: {shaderFile.Substring(inShaderLoc.Length)} ({index}/{totalCount})");
                else if (shaderFile.EndsWith(".frag"))
                    Console.WriteLine($"Compiling fragment shader: {shaderFile.Substring(inShaderLoc.Length)} ({index}/{totalCount})");
                else if (shaderFile.EndsWith(".geom"))
                    Console.WriteLine($"Compiling geometry shader: {shaderFile.Substring(inShaderLoc.Length)} ({index}/{totalCount})");
                else
                    Console.WriteLine($"Compiling miscellaneous shader: {shaderFile.Substring(inShaderLoc.Length)} ({index}/{totalCount})");
                try {     
                    Process.Start(compiler, $"{inShaderLoc}{shaderFile.Substring(inShaderLoc.Length)} -o {outShaderLoc}{shaderFile.Substring(inShaderLoc.Length)}.spv");
                }
                catch {
                    Console.WriteLine("glslc.exe not found");
                    ThrownErr();
                }
            }
        }

        static void ThrownErr() {
            Console.WriteLine("\nPress any key to continue...");
            Console.ReadLine();
            return;
        }

    }
}

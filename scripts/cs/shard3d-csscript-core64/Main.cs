using Shard3D.Core;
using Shard3D.Core.Components;
using System;

namespace Shard3D.Scripts
{  
    public class Main
    {
        public Main()
        {
            Vector4 val;

            InternalCalls.Log("Successfully loaded C#", LogSeverity.Debug);
            Console.WriteLine("fail");
        }

        public void FuncCall(float input)
        {
            InternalCalls.Log($"C# float: {input}", LogSeverity.Debug);
        }

        ~Main()
        {
            InternalCalls.Log("Goodbye world!", LogSeverity.Debug);
        }
    }
}
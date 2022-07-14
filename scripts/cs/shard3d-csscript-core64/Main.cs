using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Shard3D.CSScripts
{
    public class Main
    {
        public Main()
        {
            Console.WriteLine("Successfully loaded C#");
        }

        public void FuncCall(float input)
        {
            Console.WriteLine($"C# float: {input}");
        }

        ~Main()
        {
            Console.WriteLine("Goodbye world!");
        }
    }
}
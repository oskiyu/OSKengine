using System;
using System.Collections.Generic;

namespace ScenerySystemWIP {
    class Program {
        static void Main(string[] args) {
            Console.WriteLine("Hello World!");

            string source = @"
version -1            
skybox abcdesf
terrain path/terrain.png
 
place Cube cube1 ({ 0, 1, 0} {0, 2, 0} { 1, 1, 1 })
                ";

            Scanner sc = new Scanner();

            var xd = sc.GetTokens(source);

            foreach (var v in xd) {
                Console.WriteLine(v.ToString());
            }

        }

    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using OSKengine_CS;

namespace OSKengine_CS {
    class Program {
        static void Main(string[] args) {

            Console.WriteLine(EngineInfo.Name + " " + EngineInfo.FullVersion);
            Console.WriteLine("Wrapper: " + EngineInfo.CS_WrapperBuild);
            Console.WriteLine();

            Color color = new Color(0.5f);
            Console.WriteLine(color.Red);

            color *= 0.5f;
            Console.WriteLine(color.Alpha);
            Console.WriteLine(color);

            Vector2d vectorF = new Vector2d(0.1, 0.2);

            vectorF.X++;
            
            Console.WriteLine(vectorF);

            Console.WriteLine();
            Vector3f vec3 = new Vector3f(2, 0, 2);
            Console.WriteLine(vec3);
            Console.WriteLine(vec3.GetNormalized());
            
            Console.ReadKey();
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ScenerySystemWIP {
   
    public interface IToken {

        public void ProcessData(Scanner sc);

    }

    public class PlaceToken : IToken {

        public string className;
        public string instanceName;
        public string transform;

        public void ProcessData(Scanner sc) {
            className = sc.ProcessText();
            instanceName = sc.ProcessText();
            transform = sc.ProcessDataToken();
        }

        public override string ToString() {
            return "Place class " + className + " in " + transform + " as " + instanceName;
        }

    }

    public class VersionToken : IToken {

        public int version;

        public void ProcessData(Scanner sc) {
            version = Int32.Parse(sc.ProcessText());
        }

        public override string ToString() {
            return "Version " + version;
        }

    }

    public class SkyboxToken : IToken {

        public string path;

        public void ProcessData(Scanner sc) {
            path = sc.ProcessText();
        }

        public override string ToString() {
            return "Skybox loaded from " + path;
        }

    }

    public class TerrainToken : IToken {

        public string path;

        public void ProcessData(Scanner sc) {
            path = sc.ProcessText();
        }

        public override string ToString() {
            return "Terrain loaded from " + path;
        }

    }

}

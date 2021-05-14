using System;
using System.Collections.Generic;
using System.Globalization;
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

        public Vector3Token firstTransform = new Vector3Token();

        public void ProcessData(Scanner sc) {
            className = sc.ProcessText();
            instanceName = sc.ProcessText();

            string data = sc.ProcessDataToken();

            firstTransform.ProcessData(data);
        }

        public override string ToString() {
            return "Place class " + className + " in " + firstTransform + " as " + instanceName;
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

    public class Vector3Token {

        int position = 0;
        string data;

        char NextChar() {
            return data[position];
        }

        public void ProcessData(string data) {
            this.data = data;

            ConsumeSpace();
            x = GetNextNumber();
            ConsumeSpace();
            position++;
            ConsumeSpace();
            y = GetNextNumber();
            ConsumeSpace();
            position++;
            ConsumeSpace();
            z = GetNextNumber();
        }

        float GetNextNumber() {
            string thisData = "";

            while (Scanner.IsNumber(NextChar())) {
                thisData += NextChar();
                position++;
            }

            return float.Parse(thisData, CultureInfo.InvariantCulture);
        }

        void ConsumeSpace() {
            while (NextChar() == ' ')
                position++;
        }

        public override string ToString() {
            return "{ " + x + ", " + y + ", " + z + " }";
        }

        public float x;
        public float y;
        public float z;

    }

}

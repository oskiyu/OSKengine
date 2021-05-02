using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ScenerySystemWIP {
	/// <summary>
	/// Una clase que escanea el código fuente, generando una lista de tokens.
	/// </summary>
	public class Scanner {

		/// <summary>
		/// Código fuente.
		/// </summary>
		private string SourceCode = "";

		/// <summary>
		/// Tokens generados.
		/// </summary>
		private List<IToken> Tokens = new List<IToken>();

		/// <summary>
		/// Apunta al caracter que se está evaluando en un momento dado.
		/// </summary>
		private int CurrentChar = 0;

		/// <summary>
		/// Línea que se setá evaluando en un momento dado.
		/// </summary>
		private int Linea = 0;


		/// <summary>
		/// Procesa un código fuente, generando tokens.
		/// </summary>
		/// <param name="code">Código fuente.</param>
		/// <returns>Tokens del código fuente.</returns>
		public List<IToken> GetTokens(string code) {
			Clear();
			SourceCode = code;

			while (CurrentChar < SourceCode.Length) {
				AddNextToken();
			}

			return Tokens;
		}



		/// <summary>
		/// Finaliza el análisis del código, quedando listo para usarse con otro archivo.
		/// </summary>
		private void Clear() {
			Tokens.Clear();
			CurrentChar = 0;
			Linea = 0;
		}

		/// <summary>
		/// Procesa el siguiente token.
		/// </summary>
		private void AddNextToken() {
			string text = ProcessText();

			if (text == "place") {
				PlaceToken token = new PlaceToken();
				token.ProcessData(this);

				Tokens.Add(token);
			}

			if (text == "version") {
				VersionToken version = new VersionToken();
				version.ProcessData(this);

				Tokens.Add(version);
			}
			if (text == "skybox") {
				SkyboxToken token = new SkyboxToken();
				token.ProcessData(this);

				Tokens.Add(token);
			}
			if (text == "terrain") {
				SkyboxToken token = new SkyboxToken();
				token.ProcessData(this);

				Tokens.Add(token);
			}

		}

		public string ProcessText() {
			string output = "";

			while (PeekNextChar() == ' ')
				CurrentChar++;

			while (PeekNextChar() != ' ') {
				if (PeekNextChar() == '\0')
					return output;

				if (PeekNextChar() == '\n') {
					CurrentChar++;

					return output;
				}

				output += PeekNextChar();
				CurrentChar++;
			}

			return output;
		}

		public string ProcessDataToken() {
			string data = "";

			while (PeekNextChar() != '(')
				CurrentChar++;

			CurrentChar++;

			while (PeekNextChar() != ')') {
				data += PeekNextChar();
				CurrentChar++;
			}

			return data;
		}

		/// <summary>
		/// Devuelve el valor del próximo char, si hay.
		/// </summary>
		/// <returns>El valor del próximo char.</returns>
		public char PeekNextChar() {
			if (CurrentChar >= SourceCode.Length)
				return '\0';

			return SourceCode[CurrentChar];
		}

		/// <summary>
		/// Comprueba si el siguiente char es del tipo dado. Si lo es, hace avanzar CurrentChar.
		/// </summary>
		/// <param name="expected">Char esperado.</param>
		/// <returns>Si coincide con el siguiente char.</returns>
		public bool IsNext(char expected) {
			if (CurrentChar >= SourceCode.Length || CurrentChar + 1 >= SourceCode.Length)
				return false;

			if (SourceCode[CurrentChar] != expected)
				return false;

			CurrentChar++;

			return true;
		}

	}

}

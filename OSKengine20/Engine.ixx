export module OSKengine;

export import OSKengine.Logger;
export import OSKengine.WindowAPI;
export import OSKengine.Graphics.IRenderer;

import OSKengine.Memory.UniquePtr;

export namespace OSK {

	class Engine {

	public:

		static Window* GetWindow();
		static IRenderer* GetRenderer();
		static Logger* GetLogger();

	private:

		static Window window;
		static UniquePtr<IRenderer> renderer;
		static Logger logger;

	};

};

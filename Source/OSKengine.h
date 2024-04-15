#pragma once

#include "OSKmacros.h"

#include "UniquePtr.hpp"
#include "Version.h"
#include <string>

#include "Logger.h"
#include "IDisplay.h"
#include "IRenderer.h"
#include "AssetManager.h"
#include "EntityComponentSystem.h"
#include "IAudioApi.h"
#include "IUserInput.h"
#include "InputManager.h"
#include "Console.h"
#include "JobSystem.h"
#include "Uuid.h"


namespace OSK {
	
	namespace GRAPHICS {
		enum class RenderApiType;
	}


	/// @brief Clase principal de OSKengine.
	class OSKAPI_CALL Engine {

	public:

		Engine() = delete;
		Engine(const Engine&) = delete;
		Engine(Engine&&) = delete;

		Engine& operator=(const Engine& e) = delete;
		Engine& operator=(Engine&& e) = delete;


		/// @brief Inicializa la instancia global del motor.
		/// @param type Tipo de API de renderizado a usar durante esta ejecución.
		/// No podrá cambiarse más adelante.
		static void Create(GRAPHICS::RenderApiType type);

		/// @brief Cierra la instancia del motor.
		/// @pre El motor debe haber sido previamente inicializado con Create.
		static void Close();


		/// @return Número de segundos transcurridos desde el incio de ejecución.
		/// @threadsafe
		static float GetCurrentTime();


		/// @return Puntero al logger del motor.
		/// @stablepointer
		/// @pre El motor debe haber sido previamente inicializado con Create.
		/// @threadsafety La llamada a esta función es thread-safe, pero
		/// llamadas a funciones de la instancia devuelta pueden no serlo.
		static IO::Logger* GetLogger();

		/// @return Puntero a la consola por defecto del motor.
		/// @stablepointer
		/// @pre El motor debe haber sido previamente inicializado con Create.
		/// @threadsafety La llamada a esta función es thread-safe, pero
		/// llamadas a funciones de la instancia devuelta pueden no serlo.
		static IO::Console* GetConsole();

		/// @brief Devuelve un puntero al display del juego.
		/// @stablepointer
		/// @pre El motor debe haber sido previamente inicializado con Create.
		/// @threadsafety La llamada a esta función es thread-safe, pero
		/// llamadas a funciones de la instancia devuelta pueden no serlo.
		static IO::IDisplay* GetDisplay();

		/// @brief Devuelve un puntero al renderizador del motor.
		/// @stablepointer
		/// @pre El motor debe haber sido previamente inicializado con Create.
		/// @warning El renderizador no será usable hasta que no se llame a IRenderer::Initialize.
		/// @threadsafety La llamada a esta función es thread-safe, pero
		/// llamadas a funciones de la instancia devuelta pueden no serlo.
		static GRAPHICS::IRenderer* GetRenderer();

		/// <summary> Devuelve un puntero al sistema de assets del motor. </summary>
		/// @stablepointer
		/// @pre El motor debe haber sido previamente inicializado con Create.
		/// @threadsafety La llamada a esta función es thread-safe, pero
		/// llamadas a funciones de la instancia devuelta pueden no serlo.
		static ASSETS::AssetManager* GetAssetManager();

		/// <summary> Devuelve un puntero al ECS del motor. </summary>
		/// @stablepointer
		/// @pre El motor debe haber sido previamente inicializado con Create.
		/// @threadsafety La llamada a esta función es thread-safe, pero
		/// llamadas a funciones de la instancia devuelta pueden no serlo.
		static ECS::EntityComponentSystem* GetEcs();

		/// @brief Devuelve un puntero a los estados del input del motor.
		/// @stablepointer
		/// @pre El motor debe haber sido previamente inicializado con Create.
		/// @threadsafety La llamada a esta función es thread-safe, pero
		/// llamadas a funciones de la instancia devuelta pueden no serlo.
		static IO::IUserInput* GetInput();

		/// <summary> Devuelve un puntero al sistema de input del motor. </summary>
		/// @stablepointer
		/// @pre El motor debe haber sido previamente inicializado con Create.
		/// @threadsafety La llamada a esta función es thread-safe, pero
		/// llamadas a funciones de la instancia devuelta pueden no serlo.
		static IO::InputManager* GetInputManager();

		/// @brief Devuelve un puntero al sistema de input del motor.
		/// @stablepointer
		/// @pre El motor debe haber sido previamente inicializado con Create.
		/// @threadsafety La llamada a esta función es thread-safe, pero
		/// llamadas a funciones de la instancia devuelta pueden no serlo.
		static AUDIO::IAudioApi* GetAudioApi();

		/// @brief Devuelve un puntero al sistema de trabajos multihilo.
		/// @stablepointer
		/// @pre El motor debe haber sido previamente inicializado con Create.
		/// @threadsafe
		static JobSystem* GetJobSystem();

		/// @brief Devuelve un puntero al sistema de asignación de UUIDs.
		/// @stablepointer
		/// @pre El motor debe haber sido previamente inicializado con Create.
		/// @threadsafe
		static UuidProvider* GetUuidProvider();


		/// @return Devuelve la versión del motor. 
		/// @threadsafe
		static Version GetVersion();

		/// @return Devuelve la build del motor, en formato aaaa.mm.dd.n
		/// @threadsafe
		static std::string_view GetBuild();

		static void RegisterBuiltinAssets();
		static void RegisterBuiltinComponents();
		static void RegisterBuiltinSystems();
		static void RegisterBuiltinEvents();
		static void RegisterBuiltinVertices();
		static void RegisterBuiltinJobs();

		/// @return Índice del fotograma actual del juego,
		/// medido desde que se inició el juego en esta sesión.
		/// @threadsafe
		static UIndex64 GetCurrentGameFrameIndex();

		static void Update();

	private:

		static UniquePtr<IO::Logger> logger;
		static UniquePtr<IO::Console> console;
		static UniquePtr<IO::IDisplay> display;
		static UniquePtr<GRAPHICS::IRenderer> renderer;
		static UniquePtr<ASSETS::AssetManager> assetManager;
		static UniquePtr<ECS::EntityComponentSystem> entityComponentSystem;
		static UniquePtr<IO::IUserInput> input;
		static UniquePtr<IO::InputManager> inputManager;
		static UniquePtr<AUDIO::IAudioApi> audioApi;
		static UniquePtr<JobSystem> m_jobSystem;
		static UniquePtr<UuidProvider> uuidProvider;

		static UIndex64 gameFrameIndex;

	};

}

// #define OSK_USE_FORWARD_RENDERER
#define OSK_USE_DEFERRED_RENDERER
// #define OSK_USE_HYBRID_RENDERER
// #define OSK_NO_DEFAULT_RENDERER
// #define OSK_USE_GDR_RENDERER

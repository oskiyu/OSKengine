#pragma once

#include <OSKengine/Game.h>

class MyGame : public Game {

public:
	
	/// <summary>
	/// Primera función que se llama al crear el juego.
	/// Úsese para esablecer los ajustes del renderizador y de la ventana.
	/// </summary>
	void OnCreate() override;

	/// <summary>
	/// Úsese para cargar los elementos globales.
	/// </summary>
	void LoadContent() override;

	/// <summary>
	/// Función que se llama al cerrar el juego.
	/// </summary>
	void OnExit() override;

	/// <summary>
	/// Función que se llama una vez por tick.
	/// </summary>
	void OnTick(deltaTime_t deltaTime) override;

	/// <summary>
	/// Úsese para actualizar los spritebatches.
	/// Se llama después del renderizado 3D.
	/// </summary>
	void OnDraw2D() override;

};

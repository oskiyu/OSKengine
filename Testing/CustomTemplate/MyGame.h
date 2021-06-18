#pragma once

#include <OSKengine/Game.h>

class MyGame : public Game {

public:
	
	/// <summary>
	/// Primera funci�n que se llama al crear el juego.
	/// �sese para esablecer los ajustes del renderizador y de la ventana.
	/// </summary>
	void OnCreate() override;

	/// <summary>
	/// �sese para cargar los elementos globales.
	/// </summary>
	void LoadContent() override;

	/// <summary>
	/// Funci�n que se llama al cerrar el juego.
	/// </summary>
	void OnExit() override;

	/// <summary>
	/// Funci�n que se llama una vez por tick.
	/// </summary>
	void OnTick(deltaTime_t deltaTime) override;

	/// <summary>
	/// �sese para actualizar los spritebatches.
	/// Se llama despu�s del renderizado 3D.
	/// </summary>
	void OnDraw2D() override;

};

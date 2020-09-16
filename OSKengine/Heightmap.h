#pragma once

#include <cstdint>

namespace OSK {

	//Representa un heightmap.
	//Para su uso en Terrain.
	struct Heightmap {

		//Crea el heightmap.
		Heightmap() {

		}

		//Destruye el heightmap y su informaci�n.
		~Heightmap() {
			if (Data != nullptr)
				delete[] Data;
		}

		//P�xeles del heightmap
		uint8_t* Data = nullptr;

		//Tama�o (en p�xeles) del heightmap.
		Vector2ui Size = { 0, 0 };

	};

}

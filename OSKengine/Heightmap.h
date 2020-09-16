#pragma once

#include <cstdint>

namespace OSK {

	//Representa un heightmap.
	//Para su uso en Terrain.
	struct Heightmap {

		//Crea el heightmap.
		Heightmap() {

		}

		//Destruye el heightmap y su información.
		~Heightmap() {
			if (Data != nullptr)
				delete[] Data;
		}

		//Píxeles del heightmap
		uint8_t* Data = nullptr;

		//Tamaño (en píxeles) del heightmap.
		Vector2ui Size = { 0, 0 };

	};

}

# OSKengine Scene File (.sc) Specification

## Version 0

- Extensi�n del archivo: `.sc`.

### Comandos disponibles:

#### version

Especifica la versi�n de ScenerySystem (0).

#### skybox

Carga el skybox de la escena.

- Uso: `skybox <path>`

- Par�metros:
	- Path: ruta del skybox (de acuerdo con la spec de `ContentManager` de la versi�n de OSKengine usada).

#### terrain

Carga el terreno de la escena.

- Uso: `terrain <path>`

- Par�metros:
	- Path: ruta del heightmap (de acuerdo con la spec de `ContentManager` de la versi�n de OSKengine usada).

#### place

Spawnea una instancia de un GameObject.

- Uso: `place <className> <instanceName> <posici�n> <eje> <�ngulo> <escala>`.

- Par�metros:
	- ClassName: nombre de la clase derivada de GameObject. Debe ser una clase registrada como `OSK_GAME_OBJECT` en el proyecto de OSKengine.
	- InstanceName: nombre de la instancia spawneada por este comando. Si no es �nica, reemplazar� a la anterior en elmapa de la escena.
	- Posici�n: (tipo: `vector 3D`) posici�n en el mundo.
	- Eje: (tipo: `vector 3D`) eje sobre el que se aplica la rotaci�n (respecto al mundo).
	- �ngulo: �ngulo que se rota.
	- Escala: (tipo: `vector 3D`) escala del transform.
			

### Tipos de datos:

#### Vector 3D

Vector 3D.

- Uso: `( <x>, <y>, <z> )`. 

- Par�metros:
	- X: n�mero real.
	- Y: n�mero real.
	- Z: n�mero real.
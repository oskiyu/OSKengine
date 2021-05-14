# OSKengine Scene File (.sc) Specification

## Version 0

- Extensión del archivo: `.sc`.

### Comandos disponibles:

#### version

Especifica la versión de ScenerySystem (0).

#### skybox

Carga el skybox de la escena.

- Uso: `skybox <path>`

- Parámetros:
	- Path: ruta del skybox (de acuerdo con la spec de `ContentManager` de la versión de OSKengine usada).

#### terrain

Carga el terreno de la escena.

- Uso: `terrain <path>`

- Parámetros:
	- Path: ruta del heightmap (de acuerdo con la spec de `ContentManager` de la versión de OSKengine usada).

#### place

Spawnea una instancia de un GameObject.

- Uso: `place <className> <instanceName> <posición> <eje> <ángulo> <escala>`.

- Parámetros:
	- ClassName: nombre de la clase derivada de GameObject. Debe ser una clase registrada como `OSK_GAME_OBJECT` en el proyecto de OSKengine.
	- InstanceName: nombre de la instancia spawneada por este comando. Si no es única, reemplazará a la anterior en elmapa de la escena.
	- Posición: (tipo: `vector 3D`) posición en el mundo.
	- Eje: (tipo: `vector 3D`) eje sobre el que se aplica la rotación (respecto al mundo).
	- Ángulo: ángulo que se rota.
	- Escala: (tipo: `vector 3D`) escala del transform.
			

### Tipos de datos:

#### Vector 3D

Vector 3D.

- Uso: `( <x>, <y>, <z> )`. 

- Parámetros:
	- X: número real.
	- Y: número real.
	- Z: número real.
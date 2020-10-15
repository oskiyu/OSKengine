# OSKengine Version History.

## Alpha 0 (2019.12.3a)

###### WindowAPI:
- WindowAPI.
    - Interfaz con GLFW.
    - Carga de OpenGL.
    - Actualiza los valores de un KeyboardState.
- Keyboard Input.
    - KeyboardState.

###### OpenGL RenderAPI:
- RenderAPI.
    - Inicializa OpenGL.
    - Opciones disponibles: `BLEND`, `CULL_FACE`, `DEPTH_TEST`, `STENCIL_TEST`, `MULTISAMPLE`.
    - A�n no es funcional.
- Shader.
    - Interfaz con un shader de `GLSL`.
    - Carga a partir de un archivo.
    - Soporta los tipos: `bool`, `int`, `float`, `mat4`, `vec2`, `vec3`, `vec4`.
    - Funciones especiales para las matrices `View`, `Projection` y `Model`.

###### Types:
- `result_t` (`bool`).
- `keyState_t` (`bool`).
- `keyCode_t` (`int`).

###### Util:
- DebugOutput.
    - Muestra un mensaje por la consola.
- Exit. 
    - Termina la ejecuci�n mostrando un mensaje.


## Alpha 1 (2019.12.4a)

###### WindowAPI:
- Mouse Input.
    - MouseState:
        - Mantiene informaci�n sobre la posici�n y los botones pulsados en el rat�n.
        - Tambi�n matiene informaci�n del scroll.
- WindowAPI:
    - Se puede establecer la versi�n de OpenGL.
    - Tiene su propio MouseState actualizado.
        - Puede actualizar otros MouseState.
        
###### OpenGL RenderAPI:
- Vertex: posici�n, vector normal y TexCoords.
- Mesh.
    - Se crea a partir de una lista de v�rtices.
    - Se dibuja a si mismo.
- Model.
    - Se carga a trav�s de un archivo (**a�n no implementado**):
    - Tiene su propia matriz modelo, con su propia posici�n, rotaci�n (**a�n no implementado**) y escala.
- Camera3D.
    - Tiene la capacidad de girar y moverse por si misma.
    - Se maneja mediante el `enum CameraDirection`.
    
###### ContentAPI:
- ContentAPI (**a�n no implementado**).

###### Types:
A�adidos `typedef`s para algunas variables de OpenGL y para las variables del MouseState.
- `vertexIndex_t` (`uint32_t `).
- `vertexArrayObject_t` (`uint32_t `).
- `bufferObject_t` (`uint32_t `).
- `cameraVar_t` (`oskFloat_t `).
- `deltaTime_t` (`oskFloat_t `).
- `mouseVar_t` (`oskFloat_t `).
- `buttonState_t` (`bool`).
- `buttonCode_t` (`int32_t `).

###### Util:
- **ELIMINADO:** `DebugOutput` (sustituido por *Log*);
- Log: 
    - Muestra el mensaje por la consola (DEBUG) o guard�ndolo en un archivo .txt (RELEASE).
    - Se guarda con el comando *"CMD: SAVE"*.

    
## Alpha 2 (2019.12.4a)


## 2020.10.13a

###### CollisionSystem:
- **ELIMINADO:**: OBB.
- SAT_Collider:
    - N�mero indefinido de caras, con un n�mero indefinido de puntos cada una.
    - Deduplicaci�n de puntos.
    - Optimizaci�n de caras (para los c�lculos de ejes).
    - Optimizaci�n para elementos de posici�n/rotaci�n/escala est�ticos.
- SAT_Projection.
- SAT_CollisionInfo: c�lculo de *MTV*.

###### ContentManager:
- `std::vector` -> `std::list`.
    - Para evitar punteros desreferenciados.

###### RenderAPI:
- Terreno:
   - C�lculo m�s fiable de normales.
- PHONG SHADER SET:
    - Normales calculadas correctamente.
    - La parte specular refleja menos luz.
    - ATTENUATION m�s fiel a la luz real.
        - F�rmula: `<1.0 / (1.0 + 0.1 * distance + (1.0 / (light.infos.y * light.infos.y * 0.001)) * distance * distance)>`.
    - **OPTIMIZACI�N**: Los frags fuera del radio de luces puntuales ya no se calculan.
    - La informaci�n de las luces puntuales se transmite correctamente a los shaders.

###### Transform:
- C�lculo de matriz m�s correcto.
- Posici�n global calculada correctamente.
 
###### Types:
- Vector2, Vector3 & Vector4:
    - Operator `+=`, `-=`, `*=`, `/=`.
    - Operator `==`, `!=`.
- Nuevo `typedef`: VectorElementPtr_t.
    - Guarda el index de un elemento concreto de un `std::vector`.
- **EXPERIMENTAL**: DynamicArray<>.

###### Bugfixes:
- **Bugfix:** cargar nuevos elementos en un ContentManager no har� que se pierdan los punteros a los elementos antes cargados.
- **Bugfix:** la posici�n global se calcula correctamente para Transforms atados a otros.
- **Bugfix:** las normales del terreno se calculan mejor.
- **Bugfix:** las normales se calculan correctamente en los shader para cualquier objeto con `<model != mat4(1.0)>`.


## 2020.10.15a

###### PhysicsEngine:
- PhysicsEntity (**REWORK**):
    - Eliminados *fuerza* y *aceleraci�n angular*.
    - Ahora mantiene informaci�n sobre si puede o no moverse en los ejes X, Y y Z.
    - **WIP**: `<ApplyForce>`: aplica una fuerza sobre un punto de la entidad.
- PhysicsScene (**REWORK**):
    - Tiene informaci�n sobre el terreno (que ahora tiene un coeficiente de rozamiento).
    - Primero simula todas las entidades, despu�s resuelve colisiones.
    - Las entitdades ya no pueden tener velocidad hacia el suelo si ya est�n en la superficie del terreno.
    - La rotaci�n de las entidades disminuye con el tiempo.
    - El *MTV* de una colisi�n se aplica a ambas entidades, siempre que ninguna de ellas tenga movimiento limitado.
        - Si una de ellas tenga movimiento limitado, todo el MTV se aplica a la otra entidad.
    - Se aplica movimiento a las dos entidades de una colisi�n, dependiendo de su velocidad y masa.
    - Se aplica rotaci�n a las dos entidades de una colisi�n (rotaci�n esf�rica).

###### CollisionSystem:
- ColliderCollisionInfo.
    - Contiene informaci�n sobre una colisi�n *Collider*-*Collider*:
        - IsBroadColliding.
        - SAT_1 & SAT_2: SAT_Colliders que est�n colisionando.
- Collider:
    - Ahora calcula correctamente las colisiones *Collider*-*Collider*.
    - A�adido `<SetPosition>`, para poder establecer la posici�n del *BroadCollider* independientemente del tipo que sea.
- CollisionBox:
    - A�adido `<GetMin>` para obtener la esquina contraria a `GetTop`.
- RayCast.
    - Efectua un *raycast* contra un `SAT_Collider`.
    - A partir del origen del rayo y su direcci�n.
    - Sin l�mite de longitud.
    - Devuelve la informaci�n: 
        - Si hay intersecci�n (`<true>` o `<false>`).
        - Distancia desde el origen del rayo (si ha habido intersecci�n).
        
###### Util:
- Math:
    - A�adido `<CompareFloats>`, para comparar la igualdad de dos `float`s.

###### Bugfixes:
- **Bugfix**: *OSKengine* ya no crashea al cerrarse habiendo sido iniciado en modo **DEBUG** sin tener acceso a las capas de validaci�n.
- **Bugfix**: `Collider.IsColliding` y `Collider.GetCollisionInfo` ya calculan correctamente las colisiones.
- **Bugfix**: Las entitdades ya no pueden tener velocidad hacia el suelo si ya est�n en la superficie del terreno.
# OSKengine Version History.

## 2020.10.13a

### CollisionSystem:
- Eliminado *OBB*.
- SAT_Collider:
    - N�mero indefinido de caras, con un n�mero indefinido de puntos cada una.
    - Deduplicaci�n de puntos.
    - Optimizaci�n de caras (para los c�lculos de ejes).
    - Optimizaci�n para elementos de posici�n/rotaci�n/escala est�ticos.
- SAT_Projection.
- SAT_CollisionInfo: c�lculo de *MTV*.

### ContentManager:
- `std::vector` -> `std::list`.
    - Para evitar punteros desreferenciados.

### RenderAPI:
- Terreno:
   - C�lculo m�s fiable de normales.
- PHONG SHADER SET:
    - Normales calculadas correctamente.
    - La parte specular refleja menos luz.
    - ATTENUATION m�s fiel a la luz real.
        - F�rmula: `<1.0 / (1.0 + 0.1 * distance + (1.0 / (light.infos.y * light.infos.y * 0.001)) * distance * distance)>`.
    - **OPTIMIZACI�N**: Los frags fuera del radio de luces puntuales ya no se calculan.
    - La informaci�n de las luces puntuales se transmite correctamente a los shaders.

### Transform:
- C�lculo de matriz m�s correcto.
- Posici�n global calculada correctamente.
 
### Types:
- Vector2, Vector3 & Vector4:
    - Operator `+=`, `-=`, `*=`, `/=`.
    - Operator `==`, `!=`.
- Nuevo `typedef`: VectorElementPtr_t.
    - Guarda el index de un elemento concreto de un `std::vector`.
- **EXPERIMENTAL**: DynamicArray<>.

### Bugfixes:
-Bugfix: cargar nuevos elementos en un ContentManager no har� que se pierdan los punteros a los elementos antes cargados.
-Bugfix: la posici�n global se calcula correctamente para Transforms atados a otros.
-Bugfix: las normales del terreno se calculan mejor.
-Bugfix: las normales se calculan correctamente en los shader para cualquier objeto con `<model != mat4(1.0)>`.
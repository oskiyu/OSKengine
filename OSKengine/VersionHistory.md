# OSKengine Version History.

## 2020.10.13a

### CollisionSystem:
- Eliminado *OBB*.
- SAT_Collider:
    - Número indefinido de caras, con un número indefinido de puntos cada una.
    - Deduplicación de puntos.
    - Optimización de caras (para los cálculos de ejes).
    - Optimización para elementos de posición/rotación/escala estáticos.
- SAT_Projection.
- SAT_CollisionInfo: cálculo de *MTV*.

### ContentManager:
- `std::vector` -> `std::list`.
    - Para evitar punteros desreferenciados.

### RenderAPI:
- Terreno:
   - Cálculo más fiable de normales.
- PHONG SHADER SET:
    - Normales calculadas correctamente.
    - La parte specular refleja menos luz.
    - ATTENUATION más fiel a la luz real.
        - Fórmula: `<1.0 / (1.0 + 0.1 * distance + (1.0 / (light.infos.y * light.infos.y * 0.001)) * distance * distance)>`.
    - **OPTIMIZACIÓN**: Los frags fuera del radio de luces puntuales ya no se calculan.
    - La información de las luces puntuales se transmite correctamente a los shaders.

### Transform:
- Cálculo de matriz más correcto.
- Posición global calculada correctamente.
 
### Types:
- Vector2, Vector3 & Vector4:
    - Operator `+=`, `-=`, `*=`, `/=`.
    - Operator `==`, `!=`.
- Nuevo `typedef`: VectorElementPtr_t.
    - Guarda el index de un elemento concreto de un `std::vector`.
- **EXPERIMENTAL**: DynamicArray<>.

### Bugfixes:
-Bugfix: cargar nuevos elementos en un ContentManager no hará que se pierdan los punteros a los elementos antes cargados.
-Bugfix: la posición global se calcula correctamente para Transforms atados a otros.
-Bugfix: las normales del terreno se calculan mejor.
-Bugfix: las normales se calculan correctamente en los shader para cualquier objeto con `<model != mat4(1.0)>`.
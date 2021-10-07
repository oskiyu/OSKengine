## _class_ Camera2D

Clase que representa una cámara que existe en un mundo 2D.
A la hora de renderizar sprites y otros elementos 2D, debe usarse una cámara 2D.
Contiene un Transform2D que puede atarse a otros Transform2D.

- Namespace: `OSK`.
- Include: `<Camera2D.h>`.

### Atributos y funciones:

##### De Camera2D:

- Camera2D():
  - **DEPRECATED**.
     
- Camera2D(`WindowAPI*`_`window`_):
  - Crea una cámara, enlazada a la ventana.
  - Renderiza la escena que tiene el tamaño de la ventana.
    - Cada unidad (de posición y tamaño) de los sprites equivale a un pixel de la resolución de la pantalla.

- Update():
  - Actualiza los valores internos, apra renderizar correctamente los sprites.
    - Actualiza el transform.
    - Si no se utiliza un _targetSize_, se establece como el tamaño de la ventana.
    
- GetTranform():
  - Devuelve el Transform2D de la cámara.

- GetProjection():
  - Devuelve el `glm::ma4` que representa la matriz de proyección de la cámara.

- PointInWindowToPointInWorld(`const Vector2&`_`point`_):
    - Dado un punto en 2D de la ventana, devuelve el punto de la escena 2D al que apunta.
    - Sirve para transformar coordenadas en pantalla a coordenadas en el mundo.
  
- SetAnimation(`std::string`_`name`_):
    - Establece la animación que se reproduce.

- `bool` useTargetSize:
    - Si es **true**: cambiar la resolución de la pantalla no cambia el área de la escena 2D que capta la cámara.
    - Si es **false**: cambiar la resolución de la pantalla sí cambia el área de la escena 2D que capta la cámara.
    - Por defecto = <false>.
     
- `Vector2` targetSize:
    - Área de la escena 2D que aparecerá en la cámara.
    - Por defecto no se usa, se debe cambiar <useTargetSize>.

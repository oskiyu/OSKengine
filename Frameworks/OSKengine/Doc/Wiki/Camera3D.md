## _class_ Camera3D

Clase que representa una cámara que existe en un mundo 3DD.
A la hora de renderizar modelos 32D, debe usarse una cámara 3D.
Contiene un Transform3D que puede atarse a otros Transform3D.
Para que se puedan utilizar para renderizado, el renderizador debe actualizar sus buffers cada frame. Para esto, el renderizador debe manejar las cámaras. Debido a esto, se debe crear las cámaras mediante el renderizador, llamando a _`<RenderAPI::CreateCamera()>`_.

El renderizador tiene su propia cámara 3D que se utiliza para renderizar la escena: _`<RenderAPI::defaultCamera3D>`_.

Por ahora, no hay soporte completo para poder renderizar diferentes escenas con diferentes cámaras.

- Namespace: `OSK`.
- Include: `<Camera3D.h>`.

### Atributos y funciones:

##### De Camera3D:

- Camera3D(`cameraVar_t`_`posX`_, `cameraVar_t`_`posY`_, `cameraVar_t`_`posZ`_):
  - Crea la cámara en la posición dada.

- Camera3D(`const Vector3f&`_`position`_, `const Vector3f&`_`up`_):
  - Crea la cámara en la posición dada.
  - **Cuidado**: ahora mismo el atributo _`up`_ de este constructor no se utiliza.
     
- Girar(`double`_`xoffset`_, `double`_`yoffset`_, `bool`_`constraint`_):
  - **DEPRECATED**.
  - Para girar la cámara, girar su trasform.

- SetFov(`double`_`fov`_):
  - Establece el _field of view_ de la cámara.
    - El _field of view_ es el área del mundo 3D que la cámara puede captar.
    - Un _field of view_ menor puede funcionar como una especie de zoom.
 
- AddFov(`double`_`fov`_):
  - Cambia el _field of view_ de la cámara.
    - El _field of view_ es el área del mundo 3D que la cámara puede captar.
    - Un _field of view_ menor puede funcionar como una especie de zoom.
    
- GetProjection():
  - Devuelve el `glm::ma4` que representa la matriz de proyección de la cámara.
   
- GetView():
  - Devuelve el `glm::ma4` que representa la matriz de vista de la cámara.

- `float` fovLimitDown:
    - Límite del _field of view_ de la cámara.
      - El _field of view_ es el área del mundo 3D que la cámara puede captar.
      - Un _field of view_ menor puede funcionar como una especie de zoom.
    - Por defecto= <1.0>.

       
- `float` fovLimitUp:
    - Límite del _field of view_ de la cámara.
      - El _field of view_ es el área del mundo 3D que la cámara puede captar.
      - Un _field of view_ menor puede funcionar como una especie de zoom.
  - Por defecto= <45.0>.

     
- GetTranform():
  - Devuelve el Transform3D de la cámara.

- GetUniformBuffer():
    - Devuelve el UniformBuffer que contiene información de la cámara (matrices y posición) necesaria para renderizar una escena 2D.
      - Se debe establecer un UniformBuffer de cámara para cada modelo 3D que se vaya a renderizar.
      - Para elegir qué cámara se va a usar, se debe establecer su UniformBuffer en el material slot del modelo.
  
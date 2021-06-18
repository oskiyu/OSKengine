## _class_ AudioSystem : System

Clase que permite la reproducción de audio.

Funciona como sistema para `GameObject`s con componentes de `<SoundEmitterComponent>`.

- Namespace: `OSK`.
- Include: `<AudioAPI.h>`.
- Clase base: [OSK::ECS::System](System.md).

### Atributos y funciones:

##### Heredadas de System:

- OnCreate() _override_:
    - Inicializa la libreria de `OpenAL`.

- OnTick(`deltaTime_t`) _override_:
    - Actualiza la posición y la orientación del escuchador de acuerdo a la posición de la cámara.
    - Si no se ha establecido una cámara, no hace nada.
    
- OnRemove() _override_:
    - Cierra la libreria de `OpenAL`.
 
- GetSystemSignature() _override_:
    - Firma de componentes: { `<SoundEmitterComponent>` }.

- OnDraw() = _default_.

##### De AudioSystem:

- SetCamera3D(`<Camera3D>*`):
    - Establece la cámara que se usará como referencia para establecer la posición del escuchador.

- SetListenerSpeed(`<Vector3f>`):
    - Establece la velocidad a la que se mueve la entidad 'listener'.
    - Se usa para simular el movimiento del jugador.

- PlayAudio3D(`<SoundEmitterComponent>`, `bool <bucle>`)
    - Hace sonar un sonido (3D).
    - Si _`bucle`_ es true:
        - El sonido se reproducirá en bucle cada vez que termine.
  
- PlayAudio(`<SoundEmitterComponent>`, `bool`_`bucle`_)
    - Hace sonar un sonido (2D).
    - Si _`bucle`_ es true:
        - El sonido se reproducirá en bucle cada vez que termine.

- PauseAudio(`<SoundEmitterComponent>`)
    - Pausa el sonido.
    - Puede reanudarse con play.

- RestartAudio(`<SoundEmitterComponent>`)
    - Vuelve a reproducir un audio desde el principio.
     
### Véase también:

- [OSK::SoundEmitterComponent](SoundEmitterComponent.md).
- [OSK::Vector3](Vector3.md).
- [OSK::Camera3D](Camera3D.md).
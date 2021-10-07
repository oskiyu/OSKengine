## _class_ Collider

Un collider es un objeto que permite detectar colisiones.
Está compuesto por un broad collider (que representa un volumen muy amplio que cubre a todo el objeto) y varios SAT_Colliders que permite una detección de colisiones más precisa.
A la hora de detectar colisiones, primero se detecta si hay una colisión entre los broad colliders. Si lo hay se comprueba la colisión entre los sat colliders. Si hay alguna, ha habido una colisión.

- Namespace: `OSK`.
- Include: `<Collider.h>`.

### Atributos y funciones:

##### De Collider:

- Collider():
  - Crea un collider vacío, no funcional.

- Collider(`const Vector3f&`_`spherePosition`_, `float`_`sphereRadius`_):
  - Crea un collider cuyo broad collider es una esfera con la posición y radio dados.

- Collider(`const Vector3f&`_`boxPosition`_, `const Vector3f&`_`boxSize`_):
  - Crea un collider cuyo broad collider es una caja con la posición y tamaño dados.
     
- SetBroadCollider(`const CollisionSphere&`_`sphere`_):
  - Establece el broad collider, que es una esfera.
  
- SetBroadCollider(`const CollisionBox&`_`box`_):
  - Establece el broad collider, que es una caja.
     
- GetBroadCollisionBox():
  - Devuelve el broad collider, que es una esfera.
  
- GetBroadCollisionSphere():
  - Devuelve el broad collider, que es una caja.

- IsColliding(`Collider&`_`other`_):
  - Comprueba si dos colliders están colisionando.
  - Primero se detecta si hay una colisión entre los broad colliders. 
  - Si lo hay se comprueba la colisión entre los sat colliders.

- GetCollisionInfo(`Collider&`_`other`_):
  - Devuelve información detallada sobre la colisión (o no colisión) entre dos colliders.
    - Devuelve _`<ColliderCollisionInfo>`_.
 
- GetTransform():
  - Devuelve el transform del collider.

- AddCollider(`const Collision::SAT_Collider&`_`collider`_):
  - Añade un collider detallado al collider.
   
- `std::vector<Collision::SAT_Collider>` satColliders:
    - Colisionadores SAT más detallados.

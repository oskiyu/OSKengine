## _struct_ ColliderCollisionInfo

Estructura que contiene información detallada sobra la colisión (o no colisión) de dos colliders.

- Namespace: `OSK`.
- Include: `<ColliderCollisionInfo.h>`.

### Atributos y funciones:

##### De ColliderCollisionInfo:

- `<bool>` isColliding:
  - True si hay colisión entre los colisionadores.
   
- `<bool>` isBroadColliderColliding:
  - True si los broad colliders colisionan.
  
- `Collision::SAT_Collider*` sat1:
    - Sat collider del primer colisionador que provoca la colisión.
      - Sólo si hay colisión, de lo contrario: `<nullptr>`.
  
- `Collision::SAT_Collider*` sat2:
    - Sat collider del segundo colisionador que provoca la colisión.
      - Sólo si hay colisión, de lo contrario: `<nullptr>`.


/// @brief Comprueba si dos puntos pertenecen al mismo plano,
/// para poder decidir si una reproyección es válida.
/// @param currentPosition posición de la geometría del píxel.
/// @param previousPosition posición de la geometría del píxel en el frame histórico.
/// @param currentNormal vector normal de la geometría en el píxel.
bool SamePlaneOcclussionCheck(vec3 currentPosition, vec3 previousPosition, vec3 currentNormal) {
    // Vector contenido dentro del plano formado entre el punto anterior y el actual.
    const vec3 planeVector = currentPosition - previousPosition;

    // Para que ambos puntos pertenezcan al mismo plano, su producto dot debe ser 0.
    const float dotResult = abs(dot(planeVector, currentNormal));

    return dotResult < 0.1;
}

/// @brief Comprueba si dos puntos tienen el mismo vector normal,
/// para poder decidir si una reproyección es válida.
/// @param currentNormal vector normal de la geometría en el píxel.
/// @param previousNormal vector normal de la geometría en el píxel en el frame histórico.
bool NormalOcclussionCheck(vec3 currentNormal, vec3 previousNormal) {
    return pow(abs(dot(currentNormal, previousNormal)), 2) < 5.0;
}

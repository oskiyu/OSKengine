
float Sdf2D_Circle(vec2 point, vec2 circleCenter, float radius) {
    const float distance = length(point - circleCenter);
    return distance - radius;
}

float Sdf2D_Box(vec2 point, vec2 boxCenter, vec2 boxRadii) {
    const vec2 distances = abs(point - boxCenter) - boxRadii;
    return length(max(distances, vec2(0.0)))
        + min(
            max(distances.x, distances.y),
            0.0);
}

float Sdf2D_Segment(vec2 point, vec2 start, vec2 end) {
    const vec2 relativeStartPoint = point - start;
    const vec2 relativeEndPoint = point - end;
    return 0;
}

float Sdf2D_Hullify(float sdfValue, float thickness) {
    return abs(sdfValue) - thickness;
}

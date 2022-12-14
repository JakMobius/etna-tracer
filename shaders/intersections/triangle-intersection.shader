
const int HITTABLE_TRIANGLE_TYPE = 2;

void hittable_triangle_hit(int index, ivec4 data) {
    vec3 edge1 = uintBitsToFloat(in_buffer[index + 2].xyz);
    vec3 edge2 = uintBitsToFloat(in_buffer[index + 3].xyz);

    vec3 h = cross(ray.direction, edge2);
    float a = dot(edge1, h);

//    if(a > -epsilon && a < epsilon) return;

    vec3 point_a = uintBitsToFloat(in_buffer[index + 1].xyz);
    vec3 s = (ray.source - point_a) / a;
    float u = dot(s, h);

    if (u < 0.0 || u > 1.0) return;

    vec3 q = cross(s, edge1);
    float v = dot(ray.direction, q);

    if (v < 0.0 || u + v > 1.0) return;

    float t = dot(edge2, q);

    if (t > epsilon && t < hit_record.dist) {
        vec3 normal_a = uintBitsToFloat(in_buffer[index + 4].xyz);
        vec3 normal_b = uintBitsToFloat(in_buffer[index + 5].xyz);
        vec3 normal_c = uintBitsToFloat(in_buffer[index + 6].xyz);

        hit_record.dist = t;
        hit_record.point = ray.source + ray.direction * t;
        hit_record.normal = normalize(normal_a * (1 - u - v) + normal_b * u + normal_c * v);
        hit_record.surface_normal = normalize(cross(edge1, edge2));
        hit_record.material = data.y;
        hit_record.hittable = index;
        hit_record.front_hit = dot(hit_record.normal, ray.direction) < 0;

        if(!hit_record.front_hit) hit_record.normal = -hit_record.normal;
        if(dot(hit_record.surface_normal, ray.direction) > 0) hit_record.surface_normal = -hit_record.surface_normal;
    }
}
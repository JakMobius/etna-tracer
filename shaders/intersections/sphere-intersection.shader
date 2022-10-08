
const int HITTABLE_SPHERE_TYPE = 1;

void hittable_sphere_hit(int index, ivec4 data) {

    vec4 sphere_data = uintBitsToFloat(in_buffer[index + 1]);
    vec3 sphere_position = sphere_data.xyz;
    float sphere_radius = sphere_data.w;

    vec3 sphere_direction = ray.source - sphere_position;

    vec3 f = ray.source - sphere_position;
    float a = dot(ray.direction, ray.direction);
    float b = dot(-f, ray.direction);
    float disc = sphere_radius * sphere_radius - dot(f + b / a * ray.direction, f + b / a * ray.direction);

    if (disc < 0) return;

    disc = sqrt(disc);

    float mid = -dot(ray.direction, sphere_direction);
    float d1 = mid - disc;
    float d2 = mid + disc;
    float d = -1;

    if (d1 > 0 && (d2 > d1 || d2 < 0)) {
        d = d1;
    } else if (d2 > 0 && (d1 > d2 || d1 < 0)) {
        d = d2;
    } else {
        return;
    }

    if(d > hit_record.dist + epsilon) return;

    vec3 point = ray.source + d * ray.direction * (1 - epsilon);

    hit_record.hittable = index;
    hit_record.point = point;
    hit_record.dist = d;
    hit_record.normal = (point - sphere_position) / sphere_radius;
    hit_record.material = data.y;
    hit_record.front_hit = dot(hit_record.normal, ray.direction) < 0;

    if(!hit_record.front_hit) hit_record.normal = -hit_record.normal;
    hit_record.surface_normal = hit_record.normal;
}

uvec4 rand_indices;

// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash(uint x) {
    x += (x << 10u);
    x ^= (x >>  6u);
    x += (x <<  3u);
    x ^= (x >> 11u);
    x += (x << 15u);
    return x;
}

uvec2 hash(uvec2 x) {
    x += (x << 10u);
    x ^= (x >>  6u);
    x += (x <<  3u);
    x ^= (x >> 11u);
    x += (x << 15u);
    return x;
}

uvec3 hash(uvec3 x) {
    x += (x << 10u);
    x ^= (x >>  6u);
    x += (x <<  3u);
    x ^= (x >> 11u);
    x += (x << 15u);
    return x;
}

uvec4 hash(uvec4 x) {
    x += (x << 10u);
    x ^= (x >>  6u);
    x += (x <<  3u);
    x ^= (x >> 11u);
    x += (x << 15u);
    return x;
}

vec4 random_vec4() {
    rand_indices = hash(rand_indices);
    return rand_indices / 4294967296.0;
}

vec3 random_vec3() {
    rand_indices.xyz = hash(rand_indices.xyz);
    return rand_indices.xyz / 4294967296.0;
}

vec2 random_vec2() {
    rand_indices.xy = hash(rand_indices.xy);
    return rand_indices.xy / 4294967296.0;
}

float random_float() {
    rand_indices.x = hash(rand_indices.x);
    return rand_indices.x / 4294967296.0;
}

vec3 random_unit_vec3() {
    vec3 r = random_vec3();
    float a = r.x * pi_2;
    vec2 u = vec2(cos(a), sin(a));
    float s = r.y * 2 - 1;
    float c = sqrt(1 - s * s);
    return vec3(c * u, s);
}

void update_rand_index(int pixel_x, int pixel_y, uint seed) {
    uint hash1 = hash(hash(uint(pixel_x)) ^ uint(pixel_y)) ^ hash(seed);
    uint hash2 = hash(hash1);
    uint hash3 = hash(hash2);
    uint hash4 = hash(hash3);

    rand_indices = hash(uvec4(hash1, hash2, hash3, hash4));
}
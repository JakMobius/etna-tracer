
uint rand_index;

// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash(uint x) {
    x += (x << 10u);
    x ^= (x >>  6u);
    x += (x <<  3u);
    x ^= (x >> 11u);
    x += (x << 15u);
    return x;
}

vec3 random() {
    float x = hash(gl_GlobalInvocationID.x + rand_index);
    rand_index = hash(rand_index);
    float y = hash(gl_GlobalInvocationID.y + rand_index);
    rand_index = hash(rand_index);
    float z = hash(gl_GlobalInvocationID.z + rand_index);
    rand_index = hash(rand_index);
    return vec3(x, y, z) / 4294967296.0;
}

vec3 random_unit_vec3() {
    vec3 r = random();
    float a = r.x * pi_2;
    vec2 u = vec2(cos(a), sin(a));
    float s = r.y * 2 - 1;
    float c = sqrt(1 - s * s);
    return vec3(c * u, s);
}

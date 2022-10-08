
int env_ptr = -1;
ivec2 env_stack[max_stack_size];

void pop_environment() {
    ivec2 top_env = env_stack[env_ptr];
    while(top_env.y == -1) {
        top_env = env_stack[--env_ptr];
    }
}

void environment_toggle(int hittable_index, int material_index) {
    for (int i = env_ptr; i >= 0; i--) {
        if (env_stack[i].x == hittable_index) {
            env_stack[i].y = -1;
            pop_environment();
            return;
        }
    }

    env_stack[++env_ptr] = ivec2(hittable_index, material_index);
}
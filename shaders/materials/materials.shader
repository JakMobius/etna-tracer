
#include "./dielectric.shader"
#include "./lambertian.shader"
#include "./light.shader"
#include "./metal.shader"
#include "./atmosphere.shader"

bool material_reflect() {
	int material = hit_record.material;
	ivec4 material_data = in_buffer[material];
	int material_type = material_data.r;

	switch(material_type) {
		case MATERIAL_METAL: 	  		material_metal_reflect(material, material_data); 	    return false;
		case MATERIAL_LAMBERTIAN: 		material_lambertian_reflect(material, material_data);  return false;
		case MATERIAL_LAMBERTIAN_LIGHT: material_light_reflect(material, material_data); 	    return true;
		case MATERIAL_DIELECTRIC:
			material_dielectric_reflect(material, material_data);
			return false;
		case MATERIAL_ATMOSPHERE:
			environment_toggle(hit_record.hittable, material);
			return false;
	}

	return false;
}

bool environment_scatter() {
	ivec2 current_env = env_stack[env_ptr];

	if(current_env[0] < 0) {
		return false;
	}

	ivec4 material_data = in_buffer[current_env[1]];
	int material_type = material_data.r;

	switch(material_type) {
		case MATERIAL_ATMOSPHERE: material_atmosphere_scatter(current_env[1], material_data); return false;
		default: return false;
	}
}
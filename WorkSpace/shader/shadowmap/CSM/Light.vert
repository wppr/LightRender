
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexcoord;
								


uniform mat4 proj_light;
uniform	mat4 view_light;
uniform mat4 WorldMatrix;


void main() {
	
	gl_Position=proj_light*view_light *WorldMatrix*vec4(vPosition, 1.0);

}

uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform mat4 WorldMatrix;	




layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexcoord;						

out vec3 position_w;
out vec3 normal_w;
out vec2 texcoord;


void main()
{
	texcoord=vTexcoord;
	vec4 world_pos = WorldMatrix * vec4(vPosition,1.0);
	position_w=world_pos.xyz/world_pos.w;

	vec4 view_pos=ViewMatrix * world_pos;

	normal_w=transpose(inverse(mat3(WorldMatrix)))*(vNormal);

	gl_Position = Projection* view_pos;
}
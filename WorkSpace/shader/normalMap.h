
vec3 NormalMap2(vec3 originNormal,vec3 tangent, sampler2D normalTex, float2 texcoord) {
	vec3 bitangent = cross(originNormal, tangent);
	mat3 tbn = transpose(mat3(tangent, bitangent, originNormal));
	vec3 n;
	n.xy = -1.0 + 2.0 * textureLod(normalTex,texcoord,0).gr;
	n.z = sqrt(1.0 - n.x * n.x - n.y * n.y);
	return tbn*normalize(n);
}

vec3 NormalMap(vec3 originNormal,vec3 tangent, sampler2D normalTex, float2 texcoord) {
	vec3 bitangent = cross(originNormal, tangent);
	mat3 tbn = transpose(mat3(tangent, bitangent, originNormal));
	vec3 n;
	n.xy = -1.0 + 2.0 * textureLod(normalTex,texcoord,0).gr;
	n.z = sqrt(1.0 - n.x * n.x - n.y * n.y);
	return tbn*normalize(n);
}
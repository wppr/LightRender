in vec2 Texcoord;
in vec3 Position;
in vec3 Normal;

uniform vec4 diffuse_color;
uniform sampler2D diffuse_texture;					

out vec4 Color; 

uniform sampler2D IntegrateBRDFSampler;
uniform samplerCube EnvMap;
uniform samplerCube EnvMapDiffuse;
out vec4 color;

vec3 GetBaseColor(vec4 color,sampler2D tex){
    vec3 base_color=color.xyz;
    vec3 tex_color=texture(tex,Texcoord).rgb;
    bool HasDiffuseTexture=color.w==1.0;
    if(HasDiffuseTexture)    
        base_color=tex_color;
    return base_color;

}

uniform vec3 CameraPos;
uniform vec3 LightColor;
uniform vec3 LightPosition;

#include "brdf.h"

void main(){
	float metalness=0.1;
	float roughness=0.3;

	vec3 BaseColor=GetBaseColor(diffuse_color,diffuse_texture);
	
	vec3 DiffuseColor=mix(BaseColor,vec3(0),metalness);
	vec3 SpecularColor=mix(vec3(0.04),BaseColor,metalness);

	vec3 P=Position;
	vec3 N=normalize(Normal);
	vec3 V = normalize(CameraPos - P);
	vec3 L =normalize(LightPosition-P);
	
	vec3 direct0=StandardShading(DiffuseColor,SpecularColor, roughness, LightColor, L, V, N);

	vec3 env=EnvShading(DiffuseColor,SpecularColor, roughness, EnvMap,EnvMapDiffuse,IntegrateBRDFSampler, V, N);
	vec3 final=(env+direct0);

	color=vec4(final,1.0);

}
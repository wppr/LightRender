
in vec2 texcoord;

uniform sampler2D AlbedoTexture;
uniform sampler2D VelocityTexture;
uniform sampler2D NormalTexture;
uniform sampler2D DepthTexture;
uniform sampler2D AOTexture;
uniform sampler2D ShadowTexture;
uniform sampler2D IntegrateBRDFSampler;
uniform samplerCube EnvMap;
uniform samplerCube EnvMapDiffuse;
out vec4 color;



uniform vec3 CameraPos;
uniform mat4 ProjViewInvMatrix;
uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform bool useAO;
uniform bool useShadow;
uniform int RenderDebug;
#include "brdf.h"
#include "depthFunc.h"

void main(){

	vec3 BaseColor=texture(AlbedoTexture,texcoord).xyz;
	float metalness=0.1;
	vec3 DiffuseColor=mix(BaseColor,vec3(0),metalness);
	vec3 SpecularColor=mix(vec3(0.04),BaseColor,metalness);

	float depth=texture(DepthTexture,texcoord).x;
	if(depth>0.99999) discard;
	vec3 P=getPositionFromDepth(texcoord,depth,ProjViewInvMatrix);
	vec3 N=normalize(texture(NormalTexture,texcoord).xyz);
	vec3 V = normalize(CameraPos - P);
	vec3 L =normalize(LightPosition-P);
	float roughness=0.9;
	vec3 direct0=StandardShading(DiffuseColor,SpecularColor, roughness, LightColor, L, V, N);
	
	float ao=1.0;
	if(useAO){
		ao=texture(AOTexture,texcoord).x;
	}
	float shadow=1.0;
	if(useShadow){
		shadow=texture(ShadowTexture,texcoord).x;
	}

	direct0*=shadow;
	vec3 env=EnvShading(DiffuseColor,SpecularColor, roughness, EnvMap,EnvMapDiffuse,IntegrateBRDFSampler, V, N);
	vec3 final=(env+direct0)*ao;

	if(RenderDebug==1){
		final=direct0*ao;
	}
	if(RenderDebug==2){
		final=env*ao;
	}

	color=vec4(final,1.0);


	//color=vec4(ao);
	//color=vec4(P,1.0);
}
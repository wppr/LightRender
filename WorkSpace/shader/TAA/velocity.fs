
in vec3 position_w, normal_w;
in vec2 texcoord;
in vec3 Position;


uniform vec3 LightColor;
uniform vec3 LightPosition;

uniform vec3 eyepos;
       
uniform vec4 diffuse_color;
uniform sampler2D diffuse_texture;          

uniform vec4 specular_color;
uniform sampler2D specular_texture;

uniform vec4 ambient_color;
uniform sampler2D ambient_texture;
uniform mat4 proj_inv;

uniform mat4 WorldMatrix; 
uniform mat4 ViewMatrix;
uniform mat4 Projection;
uniform mat4 PrevProjection;
uniform mat4 PrevView;          
uniform vec2 jitterOffset;
uniform vec2 RTSize;


out vec4 fragment_color;
out vec4 velocity;
#include "brdf.h"
#include "gamma.h"
float roughness=0.3;
float metalness=0.01;

void main()
{

  vec3 N = normalize(normal_w);
  vec3 L = normalize(LightPosition-position_w);
  vec3 V = normalize(eyepos-position_w);

  vec3 BaseColor=diffuse_color.xyz*texture(diffuse_texture, texcoord).xyz;
  vec3 DiffuseColor=mix(BaseColor,vec3(0),metalness);
  vec3 SpecularColor=mix(vec3(0.01),BaseColor,metalness);

  vec3 direct=StandardShading(DiffuseColor,SpecularColor, roughness, LightColor, L, V, N);
  direct=GammaCorrect(direct);
  fragment_color=vec4(BaseColor,1.0);

  //calc velocity
  vec4 prevPos=PrevProjection*PrevView*WorldMatrix*vec4(Position,1.0);
  prevPos.xyz/=prevPos.w;
  vec4 Pos=Projection*ViewMatrix*WorldMatrix*vec4(Position,1.0);
  Pos.xyz/=Pos.w;

  vec2 v1=(Pos.xy-prevPos.xy)*vec2(0.5);
  vec2 v2=v1*RTSize-jitterOffset;
  velocity=vec4(v2/RTSize,0,0);
  //fragment_color=velocity;
}

	

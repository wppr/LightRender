
in vec3 position_w, normal_w;
in vec2 texcoord;


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

uniform mat4 ShadowMatrix;
uniform sampler2D ShadowMap;
uniform float Near;
uniform float Far;
uniform float bias;
uniform bool LinearDepth;
uniform mat4 g_lightView;

out vec4 fragment_color;

#include "brdf.h"
#include "gamma.h"
#include "shadowMapUse.h"
#include "pcss.h"
#include "csm.h"

uniform sampler2DArray csmTex2;
float roughness=1.0;
float metalness=0.01;

void main()
{

  vec3 N = normalize(normal_w);
  vec3 L = normalize(LightPosition-position_w);
  vec3 V = normalize(eyepos-position_w);

  vec3 BaseColor=diffuse_color.xyz*texture(diffuse_texture, texcoord).xyz;
  vec3 DiffuseColor=mix(BaseColor,vec3(0),metalness);
  vec3 SpecularColor=mix(vec3(0.04),BaseColor,metalness);

  vec3 color=StandardShading(DiffuseColor,SpecularColor, roughness, LightColor, L, V, N);
 

  vec4 shadowPosition = ShadowMatrix* vec4(position_w, 1.0);
  shadowPosition.xy /= shadowPosition.w; 
  shadowPosition.z /= shadowPosition.w;
  shadowPosition.z += bias;
  vec2 uv=shadowPosition.xy;
  float z=shadowPosition.z;
  vec2 dz_duv = depthGradient(uv, z);
  float zEye = -(g_lightView * vec4(position_w,1.0)).z;
  float shadow = 1.0f;
  switch (g_shadowTechnique)
  {
      case 1:
          shadow = pcssShadow(uv, z, dz_duv, zEye);
          break;

      case 2:
          shadow = pcfShadow(uv, z, dz_duv, zEye);
          break;
      case 3:
          shadow = Shadow(ShadowMap,ShadowMatrix,position_w,bias);
          break;
      case 4:
          shadow=computeShadowCoef(vec4(position_w, 1.0),bias,gl_FragCoord.z);
          break;
  }
  if(g_shadowTechnique==4)
    color*=layerColor.xyz;
  color*=shadow;

  color=GammaCorrect(color);
  fragment_color=vec4(color,1.0);

  // vec2 siz=vec2(1280,720);
  // vec2 coord=gl_FragCoord.xy/siz;
  // float d=texture(csmTex2,vec3(coord,3)).x;
 // fragment_color=vec4(d);
}

  

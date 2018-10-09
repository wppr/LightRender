
in vec2 texcoord;


uniform sampler2D DepthTexture;
uniform mat4 ProjViewInvMatrix;

uniform mat4 ShadowMatrix;
uniform sampler2D ShadowMap;
uniform float bias;
uniform float csmbias;
uniform mat4 g_lightView;

out vec4 fragment_color;

#include "shadowMapUse.h"
#include "pcss.h"
#include "csm.h"
#include "depthfunc.h"


void main()
{

  float depth=texture(DepthTexture,texcoord).x;
  if(depth>0.9999) discard;
  vec3 position_w=getPositionFromDepth(texcoord,depth,ProjViewInvMatrix);

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
          shadow = Shadow(ShadowMap,ShadowMatrix,position_w,bias);        
          break;

      case 2:
          shadow = pcfShadow(uv, z, dz_duv, zEye);
          break;
      case 3:
          shadow = pcssShadow(uv, z, dz_duv, zEye);
          break;
      case 4:
          shadow=computeShadowCoef(vec4(position_w, 1.0),csmbias,depth);
          break;
  }
  //fragment_color=vec4(shadow*layerColor);
  fragment_color=vec4(shadow);

  // vec2 siz=vec2(1280,720);
  // vec2 coord=gl_FragCoord.xy/siz;
  // float d=texture(csmTex2,vec3(coord,3)).x;
 // fragment_color=vec4(d);
}

  

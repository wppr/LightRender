

vec3 getPositionFromDepth(vec2 uv,float depth,mat4 projview_inv_mat){
  vec3 ndc = 2.0 * vec3(uv, depth) - 1.0;
  vec4 P = projview_inv_mat * vec4(ndc, 1.0); 
  P.xyz /= P.w;
  return P.xyz;
}


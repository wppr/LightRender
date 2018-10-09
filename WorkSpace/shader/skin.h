#define MAX_BONES 100

uniform mat4 bone[MAX_BONES];

vec3 SkinPos(ivec4 boneID,vec4 weight,vec3 originPos){
	vec4 skinnedPos=vec4(0);
	skinnedPos+=bone[boneID.x]*vec4(originPos,1.0)*weight.x;
	skinnedPos+=bone[boneID.y]*vec4(originPos,1.0)*weight.y;
	skinnedPos+=bone[boneID.z]*vec4(originPos,1.0)*weight.z;
	skinnedPos+=bone[boneID.w]*vec4(originPos,1.0)*(weight.w);
	return skinnedPos.xyz/skinnedPos.w;
}

vec3 SkinNormal(ivec4 boneID,vec4 weight,vec3 originNormal){
	vec4 skinnedNormal=vec4(0);
	skinnedNormal+=bone[boneID.x]*vec4(originNormal,1.0)*weight.x;
	skinnedNormal+=bone[boneID.y]*vec4(originNormal,1.0)*weight.y;
	skinnedNormal+=bone[boneID.z]*vec4(originNormal,1.0)*weight.z;
	skinnedNormal+=bone[boneID.w]*vec4(originNormal,1.0)*(weight.w);
	return skinnedNormal.xyz/skinnedNormal.w;
}

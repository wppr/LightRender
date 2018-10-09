
uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform mat4 WorldMatrix;						

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexcoord;
layout (location = 3) in ivec4 vBoneID;
layout (location = 4) in vec4 vBoneWeight;

//uniform mat4 InstanceMatrix[50];
uniform bool useSkinning;

out vec2 Texcoord;
out vec3 Position;
out vec3 Normal;

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
	vec3 skinnedNormal=vec3(0);
	skinnedNormal+=transpose(inverse(mat3(bone[boneID.x])))*vec3(originNormal)*weight.x;
	skinnedNormal+=transpose(inverse(mat3(bone[boneID.y])))*vec3(originNormal)*weight.y;
	skinnedNormal+=transpose(inverse(mat3(bone[boneID.z])))*vec3(originNormal)*weight.z;
	skinnedNormal+=transpose(inverse(mat3(bone[boneID.w])))*vec3(originNormal)*weight.w;

	return normalize(skinnedNormal);
}


void main()
{
	//mat4 World=InstanceMatrix[gl_InstanceID];
	vec3 n=vNormal;
	vec3 p=vPosition;
	if(useSkinning){
		p=SkinPos(vBoneID,vBoneWeight,vPosition);
		n=SkinNormal(vBoneID,vBoneWeight,vNormal);
	}


	Position = (WorldMatrix * vec4(p,1.0)).xyz;	
	Texcoord=vTexcoord;
	Normal=transpose(inverse(mat3(WorldMatrix)))*(n);

	gl_Position = Projection* ViewMatrix*WorldMatrix * vec4(p,1.0);

}


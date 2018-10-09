
in vec2 Texcoord;
in vec3 Position;
in vec3 Normal;

uniform vec4 diffuse_color;
uniform sampler2D diffuse_texture;					

uniform mat4 WorldMatrix; 
uniform mat4 ViewMatrix;
uniform mat4 Projection;
uniform mat4 PrevProjection;
uniform mat4 PrevView;          
uniform vec2 jitterOffset;
uniform vec2 RTSize;
uniform bool EnableTemporalAA;

layout (location = 0) out vec4 DiffuseOut; 
layout (location = 1) out vec4 VelocityOut; 
layout (location = 2) out vec4 NormalOut; 

vec3 GetBaseColor(vec4 color,sampler2D tex){
    vec3 base_color=color.xyz;
    vec3 tex_color=texture(tex,Texcoord).rgb;
    tex_color=pow(tex_color,vec3(2.2));
    if(color.w==1.0)    
        base_color=tex_color;
    return base_color;

}

void main() 
{ 

    DiffuseOut = vec4(GetBaseColor(diffuse_color,diffuse_texture),1.0);
    NormalOut=vec4(normalize(Normal),1.0);


  //calc velocity
  if(EnableTemporalAA){
	  vec4 prevPos=PrevProjection*PrevView*vec4(Position,1.0);
	  prevPos.xyz/=prevPos.w;
	  vec4 Pos=Projection*ViewMatrix*vec4(Position,1.0);
	  Pos.xyz/=Pos.w;

	  vec2 v1=(Pos.xy-prevPos.xy)*vec2(0.5);
	  vec2 v2=v1*RTSize-jitterOffset;
	  VelocityOut=vec4(v2/RTSize,0,0);
	}
}
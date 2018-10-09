


in vec2 texcoord;
out vec3 fragout;
uniform sampler2D linearTex;
uniform float exposure;
uniform int UseTonemap;

#include "tonemap.h"
#include "gamma.h"

void main(){
	
	vec3 LinearColor=exposure*texture(linearTex,texcoord).xyz;

	vec3 tonemappedColor=ACESFilm( LinearColor ); 
	if(UseTonemap!=1)
		tonemappedColor=LinearColor;

	fragout=GammaCorrect(tonemappedColor);


	//fragout=Uncharted2TonemapFull(LinearColor,exposure);
}

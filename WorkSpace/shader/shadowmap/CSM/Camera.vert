

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

out vec3 N;
out vec3 L;
out vec3 V;
out vec4 worldPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 lightDir;

void main(void) {
    mat4 mvMatrix = viewMatrix  * modelMatrix;
    vec4 viewPos = mvMatrix * position;
    N = mat3(mvMatrix) * normal;
    L = mat3(mvMatrix) * lightDir;
    V = viewPos.xyz;
    worldPos = modelMatrix * position;
    gl_Position = projMatrix * viewPos;
}


in vec3 N;
in vec3 L;
in vec3 V;
in vec4 worldPos;

out vec4 color;

layout (binding = 0) uniform sampler2DArrayShadow shadowTex;

// Light view-projection-scale-bias matrix.
uniform mat4 lightVPSBMatrices[4];
uniform vec4 normalizedFarPlanes;
uniform vec3 ambient;
uniform vec3 diffuseAlbedo;
uniform vec3 specularAlbedo;
uniform float specularPower;

// Debug tool to emphasize different shadow maps.
vec4 layerColor;

vec3 computeShadowCoords(int slice)
{
    // Orthographic projection doesn't need division by w.
    vec4 shadowCoords = lightVPSBMatrices[slice] * worldPos;
    return shadowCoords.xyz;
}

float computeShadowCoef()
{
    int slice = 3;
    layerColor = vec4(1.0, 0.5, 1.0, 1.0);
    if (gl_FragCoord.z < normalizedFarPlanes.x) {
        slice = 0;
        layerColor = vec4(1.0, 0.5, 0.5, 1.0);
    } else if (gl_FragCoord.z < normalizedFarPlanes.y) {
        slice = 1;
        layerColor = vec4(0.5, 1.0, 0.5, 1.0);
    } else if (gl_FragCoord.z < normalizedFarPlanes.z) {
        slice = 2;
        layerColor = vec4(0.5, 0.5, 1.0, 1.0);
    }

    vec4 shadowCoords;
    // Swizzling specific for shadow sampler.
    shadowCoords.xyw = computeShadowCoords(slice);

    // Bias to reduce shadow acne.
    shadowCoords.w -= 0.001;

    shadowCoords.z = float(slice);
    return texture(shadowTex, shadowCoords);
}

void main(void) {
    vec3 nN = normalize(N);
    vec3 nL = normalize(L);
    vec3 nV = normalize(V);

    vec3 R = reflect(nL, nN);

    float cosTheta = max(dot(nN, nL), 0.0);

    vec3 diffuse = max(cosTheta, 0.0) * diffuseAlbedo;
    vec3 specular = pow(max(dot(R, nV), 0.0), specularPower) * specularAlbedo;

    color = vec4(ambient, 1.0);
    color += computeShadowCoef() * vec4(diffuse + specular, 1.0) * layerColor;
}

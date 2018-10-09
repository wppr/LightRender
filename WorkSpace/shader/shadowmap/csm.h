

uniform sampler2DArrayShadow csmTex;
// Light view-projection-scale-bias matrix.
uniform mat4 lightVPSBMatrices[4];
uniform vec4 normalizedFarPlanes;

vec4 layerColor;

vec3 computeShadowCoords(int slice,vec4 worldPos)
{
    // Orthographic projection doesn't need division by w.
    vec4 shadowCoords = lightVPSBMatrices[slice] * worldPos;
    return shadowCoords.xyz;
}

float computeShadowCoef(vec4 worldPos,float bias,float depth)
{
    int slice = 3;
    layerColor = vec4(1.0, 0.5, 1.0, 1.0);
    if (depth < normalizedFarPlanes.x) {
        slice = 0;
        layerColor = vec4(1.0, 0.5, 0.5, 1.0);
    } else if (depth < normalizedFarPlanes.y) {
        slice = 1;
        layerColor = vec4(0.5, 1.0, 0.5, 1.0);
    } else if (depth < normalizedFarPlanes.z) {
        slice = 2;
        layerColor = vec4(0.5, 0.5, 1.0, 1.0);
    }

    vec4 shadowCoords;
    // Swizzling specific for shadow sampler.
    shadowCoords.xyw = computeShadowCoords(slice,worldPos);

    // Bias to reduce shadow acne.
    shadowCoords.w += bias;

    shadowCoords.z = float(slice);
    return texture(csmTex, shadowCoords);
}
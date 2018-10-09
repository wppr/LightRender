
uniform int g_shadowTechnique; // 0 = none, 1 = pcss, 2 = pcf
uniform int g_samplePattern;
uniform vec2 g_lightRadiusUV;
uniform float g_lightZNear;
uniform float g_lightZFar;
uniform sampler2D g_shadowMapDepth;
uniform sampler2DShadow g_shadowMapPcf;

#include "poisson.h"


bool isBlack(vec3 c)
{
    return (dot(c, c) == 0.0);
}

float borderDepthTexture(sampler2D tex, vec2 uv)
{
	return ((uv.x <= 1.0) && (uv.y <= 1.0) &&
	 (uv.x >= 0.0) && (uv.y >= 0.0)) ? textureLod(tex, uv, 0.0).z : 1.0;
}
float borderPCFTexture(sampler2DShadow tex, vec3 uvz)
{
	return ((uvz.x <= 1.0) && (uvz.y <= 1.0) &&
	 (uvz.x >= 0.0) && (uvz.y >= 0.0)) ? texture(tex, uvz) : 
	 ((uvz.z <= 1.0) ? 1.0 : 0.0);
}
float shadowCompare(sampler2D tex, vec3 uvz){
    float z2=uvz.z;
    float z1=texture(tex, uvz.xy).r;
    if(z2>z1)
        return 0.0;
    else 
        return 1.0;
}
float borderPCFTexture(sampler2D tex, vec3 uvz)
{
    return ((uvz.x <= 1.0) && (uvz.y <= 1.0) &&
     (uvz.x >= 0.0) && (uvz.y >= 0.0)) ? shadowCompare(tex,uvz) : 
     ((uvz.z <= 1.0) ? 1.0 : 0.0);
}

// Using similar triangles from the surface point to the area light
vec2 searchRegionRadiusUV(float zWorld)
{
    return g_lightRadiusUV * (zWorld - g_lightZNear) / zWorld;
}

// Using similar triangles between the area light, the blocking plane and the surface point
vec2 penumbraRadiusUV(float zReceiver, float zBlocker)
{
    return g_lightRadiusUV * (zReceiver - zBlocker) / zBlocker;
}

// Project UV size to the near plane of the light
vec2 projectToLightUV(vec2 sizeUV, float zWorld)
{
    return sizeUV * g_lightZNear / zWorld;
}

// Derivatives of light-space depth with respect to texture2D coordinates
vec2 depthGradient(vec2 uv, float z)
{
    vec2 dz_duv = vec2(0.0, 0.0);

    vec3 duvdist_dx = dFdx(vec3(uv,z));
    vec3 duvdist_dy = dFdy(vec3(uv,z));

    dz_duv.x = duvdist_dy.y * duvdist_dx.z;
    dz_duv.x -= duvdist_dx.y * duvdist_dy.z;

    dz_duv.y = duvdist_dx.x * duvdist_dy.z;
    dz_duv.y -= duvdist_dy.x * duvdist_dx.z;

    float det = (duvdist_dx.x * duvdist_dy.y) - (duvdist_dx.y * duvdist_dy.x);
    dz_duv /= det;

    return dz_duv;
}

float biasedZ(float z0, vec2 dz_duv, vec2 offset)
{
    return z0 + dot(dz_duv, offset);
}

float zClipToEye(float z)
{
    return g_lightZFar * g_lightZNear / (g_lightZFar - z * (g_lightZFar - g_lightZNear));   
}

// Returns average blocker depth in the search region, as well as the number of found blockers.
// Blockers are defined as shadow-map samples between the surface point and the light.
void findBlocker(
    out float accumBlockerDepth, 
    out float numBlockers,
    out float maxBlockers,
    vec2 uv,
    float z0,
    vec2 dz_duv,
    vec2 searchRegionRadiusUV)
{
    accumBlockerDepth = 0.0;
    numBlockers = 0.0;
	maxBlockers = 300.0;
    
    switch (g_samplePattern)
    {
        case POISSON_25_25:
        {
            maxBlockers = 25.0;
            for (int i = 0; i < 25; ++i)
            {
                vec2 offset = Poisson25[i] * searchRegionRadiusUV;
                float shadowMapDepth = borderDepthTexture(g_shadowMapDepth, uv + offset);
                float z = biasedZ(z0, dz_duv, offset);
                if (shadowMapDepth < z)
                {
                    accumBlockerDepth += shadowMapDepth;
                    numBlockers++;
                }
            }
        }
        break;

        case POISSON_32_64:
        {
            maxBlockers = 32.0;
            for (int i = 0; i < 32; ++i)
            {
                vec2 offset = Poisson32[i] * searchRegionRadiusUV;
                float shadowMapDepth = borderDepthTexture(g_shadowMapDepth, uv + offset);
                float z = biasedZ(z0, dz_duv, offset);
                if (shadowMapDepth < z)
                {
                    accumBlockerDepth += shadowMapDepth;
                    numBlockers++;
                }
            }
        }
        break;

        case POISSON_100_100:
        {
            maxBlockers = 100.0;
            for (int i = 0; i < 100; ++i)
            {
                vec2 offset = Poisson100[i] * searchRegionRadiusUV;
                float shadowMapDepth = borderDepthTexture(g_shadowMapDepth, uv + offset);
                float z = biasedZ(z0, dz_duv, offset);
                if (shadowMapDepth < z)
                {
                    accumBlockerDepth += shadowMapDepth;
                    numBlockers++;
                }
            }
        }
        break;

        case POISSON_64_128:
        {
            maxBlockers = 64.0;
            for (int i = 0; i < 64; ++i)
            {
                vec2 offset = Poisson64[i] * searchRegionRadiusUV;
                float shadowMapDepth = borderDepthTexture(g_shadowMapDepth, uv + offset);
                float z = biasedZ(z0, dz_duv, offset);
                if (shadowMapDepth < z)
                {
                    accumBlockerDepth += shadowMapDepth;
                    numBlockers++;
                }
            }
        }
        break;

        case REGULAR_49_225:
        {
            maxBlockers = 49.0;
            vec2 stepUV = searchRegionRadiusUV / 3.0;
            for (int x = -3; x <= 3; ++x)
            {
                for (int y = -3; y <= 3; ++y)
                {
                    vec2 offset = vec2(x, y) * stepUV;
                    float shadowMapDepth = borderDepthTexture(g_shadowMapDepth, uv + offset);
                    float z = biasedZ(z0, dz_duv, offset);
                    if (shadowMapDepth < z)
                    {
                        accumBlockerDepth += shadowMapDepth;
                        numBlockers++;
                    }
                }
            }
        }
        break;
    }
}

// Performs PCF filtering on the shadow map using multiple taps in the filter region.
float pcfFilter(vec2 uv, float z0, vec2 dz_duv, vec2 filterRadiusUV)
{
    float sum = 0.0;

    switch (g_samplePattern)
    {
        case POISSON_25_25:
        {
            for (int i = 0; i < 25; ++i)
            {
                vec2 offset = Poisson25[i] * filterRadiusUV;
                float z = biasedZ(z0, dz_duv, offset);
                sum += borderPCFTexture(g_shadowMapPcf, vec3(uv + offset, z));
            }
            return sum / 25.0;
        }

        case POISSON_32_64:
        {
            for (int i = 0; i < 64; ++i)
            {
                vec2 offset = Poisson64[i] * filterRadiusUV;
                float z = biasedZ(z0, dz_duv, offset);
                sum += borderPCFTexture(g_shadowMapPcf, vec3(uv + offset, z));
            }
            return sum / 64.0;
        }

        case POISSON_100_100:
        {
            for (int i = 0; i < 100; ++i)
            {
                vec2 offset = Poisson100[i] * filterRadiusUV;
                float z = biasedZ(z0, dz_duv, offset);
                sum += borderPCFTexture(g_shadowMapPcf, vec3(uv + offset, z));
            }
            return sum / 100.0;
        }

        case POISSON_64_128:
        {
            for (int i = 0; i < 128; ++i)
            {
                vec2 offset = Poisson128[i] * filterRadiusUV;
                float z = biasedZ(z0, dz_duv, offset);
                sum += borderPCFTexture(g_shadowMapPcf, vec3(uv + offset, z));
            }
            return sum / 128.0;
        }

        case REGULAR_49_225:
        {
            vec2 stepUV = filterRadiusUV / 7.0;
            for (int x = -7; x <= 7; ++x)
            {
                for (int y = -7; y <= 7; ++y)
                {
                    vec2 offset = vec2(x, y) * stepUV;
                    float z = biasedZ(z0, dz_duv, offset);
                    sum += borderPCFTexture(g_shadowMapPcf, vec3(uv + offset, z));
                }                
            }
            float numSamples = 7.0 * 2.0 + 1.0;
            return sum / (numSamples * numSamples);
        }

        default:
            return 1.0;
    }
}

float pcssShadow(vec2 uv, float z, vec2 dz_duv, float zEye)
{
    // ------------------------
    // STEP 1: blocker search
    // ------------------------
    float accumBlockerDepth, numBlockers, maxBlockers;
    vec2 searchRegionRadiusUV = searchRegionRadiusUV(zEye);
    findBlocker(accumBlockerDepth, numBlockers, maxBlockers, uv, z, dz_duv, searchRegionRadiusUV);

    // Early out if not in the penumbra
    if (numBlockers == 0.0)
        return 1.0;

    // ------------------------
    // STEP 2: penumbra size
    // ------------------------
    float avgBlockerDepth = accumBlockerDepth / numBlockers;
    float avgBlockerDepthWorld = zClipToEye(avgBlockerDepth);
    vec2 penumbraRadius = penumbraRadiusUV(zEye, avgBlockerDepthWorld);
    vec2 filterRadius = projectToLightUV(penumbraRadius, zEye);

    // ------------------------
    // STEP 3: filtering
    // ------------------------
    return pcfFilter(uv, z, dz_duv, filterRadius);
}

float pcfShadow(vec2 uv, float z, vec2 dz_duv, float zEye)
{
    // Do a blocker search to enable early out
    float accumBlockerDepth, numBlockers, maxBlockers;
    vec2 searchRegionRadius = searchRegionRadiusUV(zEye);
    findBlocker(accumBlockerDepth, numBlockers, maxBlockers, uv, z, dz_duv, searchRegionRadius);
    if (numBlockers == 0.0)
        return 1.0;

    vec2 filterRadius = 0.1 * g_lightRadiusUV;
    return pcfFilter(uv, z, dz_duv, filterRadius);
}
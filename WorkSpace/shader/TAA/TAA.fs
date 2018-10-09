
#define float2 vec2
#define float3 vec3
#define float4 vec4
#define saturate(a) clamp(a, 0.0, 1.0)
#define lerp(a,b,w) mix(a,b,w)

in vec2 texcoord;
uniform sampler2D current;
uniform sampler2D history;
uniform sampler2D VelocityTexture;
uniform sampler2D DepthTexture;

out vec4 color;
#define ClampModes_RGB_Clamp 0
#define ClampModes_RGB_Clip 1
#define ClampModes_Variance_Clip 2
float VarianceClipGamma=1.5;
bool UseExposureFiltering=true;
bool InverseLuminanceFiltering=true;
uniform bool EnableTemporalAA;
float TemporalAABlendFactor=0.95;
bool UseTemporalColorWeighting=true;
bool UseStandardReprojection=false;
float LowFreqWeight=0.25;
float HiFreqWeight=0.85;
vec2 TextureSize;
vec2 pixelPos;
int NeighborhoodClampMode=1;

float Luminance(in float3 clr)
{
    return dot(clr, float3(0.299f, 0.587f, 0.114f));
}
 float FilterCubic(in float x, in float B, in float C)
{
    float y = 0.0f;
    float x2 = x * x;
    float x3 = x * x * x;
    if(x < 1)
        y = (12 - 9 * B - 6 * C) * x3 + (-18 + 12 * B + 6 * C) * x2 + (6 - 2 * B);
    else if (x <= 2)
        y = (-B - 6 * C) * x3 + (6 * B + 30 * C) * x2 + (-12 * B - 48 * C) * x + (8 * B + 24 * C);

    return y / 6.0f;
}
float Filter(in float x, in bool rescaleCubic)
{
	float cubicX = rescaleCubic ? x * 2.0f : x;

	return FilterCubic(cubicX, 0, 0.5f);
}
float3 ClipAABB(float3 aabbMin, float3 aabbMax, float3 prevSample, float3 avg)
{
        // note: only clips towards aabb center (but fast!)
        float3 p_clip = 0.5 * (aabbMax + aabbMin);
        float3 e_clip = 0.5 * (aabbMax - aabbMin);

        float3 v_clip = prevSample - p_clip;
        float3 v_unit = v_clip.xyz / e_clip;
        float3 a_unit = abs(v_unit);
        float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));

        if (ma_unit > 1.0)
            return p_clip + v_clip / ma_unit;
        else
            return prevSample;// point inside aabb
}

float3 Reproject(float2 pixelPos)
{
    float2 velocity =float2(0.0);

    {
        float closestDepth = 10.0f;
        for(int vy = -1; vy <= 1; ++vy)
        {
            for(int vx = -1; vx <= 1; ++vx)
            {
                float2 uv=texcoord+float2(vx, vy)/TextureSize;
                float2 neighborVelocity = texture(VelocityTexture, uv).xy;
                float neighborDepth = texture(DepthTexture, uv).x;
                if(neighborDepth < closestDepth)
                {
                    velocity = neighborVelocity;
                    closestDepth = neighborDepth;
                }

            }
        }
    }
    //velocity=texture(VelocityTexture,texcoord).xy;

    float2 reprojectedPos = pixelPos- velocity*TextureSize;
    float2 reprojectedUV = reprojectedPos / TextureSize;
    //return texture(history,texcoord).xyz;
    if(UseStandardReprojection)
    {
        return texture(history,reprojectedUV).xyz;
    }
    else
    {
        float3 sum = vec3(0.0);
        float totalWeight = 0.0f;

        for(int ty = -1; ty <= 2; ++ty)
        {
            for(int tx = -1; tx <= 2; ++tx)
            {
                float2 samplePos = floor(reprojectedPos + float2(tx, ty)) + 0.5f;
                float3 reprojectedSample = texelFetch(history,ivec2(samplePos),0).xyz;

                float2 sampleDist = abs(samplePos - reprojectedPos);
                float filterWeight = Filter(sampleDist.x, false) *
                                     Filter(sampleDist.y, false);

                float sampleLum = Luminance(reprojectedSample);

                // if(UseExposureFiltering)
                //     sampleLum *= exp2(ManualExposure - ExposureScale + ExposureFilterOffset);

                if(InverseLuminanceFiltering)
                    filterWeight *= 1.0f / (1.0f + sampleLum);

                sum += reprojectedSample * filterWeight;
                totalWeight += filterWeight;
            }
        }

        return max(sum / totalWeight, 0.0f);
    }

}
void main() {
	TextureSize=vec2(textureSize(current,0));
	pixelPos=TextureSize*texcoord;
	float3 sum = vec3(0.0f);
    float totalWeight = 0.0f;

    float3 clrMin = vec3(99999999.0);
    float3 clrMax = vec3(-99999999.0);

    float3 m1 = vec3(0.0);
    float3 m2 = vec3(0.0);
    float mWeight = 0.0f;
    const int SampleRadius_ = 1;
    const float filterRadius = 1.0;

      for(int y = -SampleRadius_; y <= SampleRadius_; ++y)
    {
        for(int x = -SampleRadius_; x <= SampleRadius_; ++x)
        {
            float2 sampleOffset = float2(x, y);
            float2 samplePos = pixelPos + sampleOffset;
            samplePos = clamp(samplePos, vec2(0.0), TextureSize - 1.0f);
        
            float2 sampleDist=abs(sampleOffset)*2;
            float3 Sample = texture(current, samplePos/TextureSize).xyz;
            Sample = max(Sample, vec3(0.0f));

            float weight = Filter(sampleDist.x, true) *
                           Filter(sampleDist.y, true);
            clrMin = min(clrMin, Sample);
            clrMax = max(clrMax, Sample);

            float sampleLum = Luminance(Sample);

            // if(UseExposureFiltering)
            //     sampleLum *= exp2(ManualExposure - ExposureScale + ExposureFilterOffset);

            if(InverseLuminanceFiltering)
                weight *= 1.0f / (1.0f + sampleLum);

            sum += Sample * weight;
            totalWeight += weight;

            m1 += Sample;
            m2 += Sample * Sample;
            mWeight += 1.0f;            

        }
    }

    float3 outcolor = texture(current, texcoord).xyz;
    outcolor = max(outcolor, vec3(0.0));

    if(EnableTemporalAA)
    {
        float3 currColor = outcolor;
        float3 prevColor = Reproject(pixelPos);

        if(NeighborhoodClampMode == ClampModes_RGB_Clamp)
        {
            prevColor = clamp(prevColor, clrMin, clrMax);
        }
        else if(NeighborhoodClampMode == ClampModes_RGB_Clip)
        {
            prevColor = ClipAABB(clrMin, clrMax, prevColor, m1 / mWeight);
        }
        else if(NeighborhoodClampMode == ClampModes_Variance_Clip)
        {
            float3 mu = float3(m1 / mWeight);
            float3 sigma = sqrt(abs(m2 / mWeight - mu * mu));
            float3 minc = mu - VarianceClipGamma * sigma;
            float3 maxc = mu + VarianceClipGamma * sigma;
            prevColor = ClipAABB(minc, maxc, prevColor, mu);
        }


        float3 weightA = vec3(saturate(1.0f - TemporalAABlendFactor));
        float3 weightB = vec3(saturate(TemporalAABlendFactor));

        // if(UseTemporalColorWeighting)
        // {
        //     float3 temporalWeight = saturate(abs(clrMax - clrMin) / currColor);
        //     weightB = saturate(lerp(vec3(LowFreqWeight), vec3(HiFreqWeight), temporalWeight));
        //     weightA = 1.0f - weightB;
        // }

        if(InverseLuminanceFiltering)
        {
            weightA *= 1.0f / (1.0f + Luminance(currColor));
            weightB *= 1.0f / (1.0f + Luminance(prevColor));
        }

        outcolor = (currColor * weightA + prevColor * weightB) / (weightA + weightB);
    	//outcolor = prevColor;

    }
    color=vec4(outcolor,1);
    //color=texture(velocity,texcoord)*500;
}
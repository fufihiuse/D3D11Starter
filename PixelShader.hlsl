#include "ShaderInclude.hlsli"
#define MAX_LIGHTS 128

cbuffer ExternalData : register(b0)
{
    float2 uvScale;
    float2 uvOffset;
    float3 cameraPos;
    int lightCount;
    Light lights[MAX_LIGHTS];
}

Texture2D Albedo : register(t0);
Texture2D NormalMap : register(t1);
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);
SamplerState BasicSampler : register(s0); // "s" registers for samplers

float3 CalculateViewVector(float3 worldPosition)
{
    return normalize(cameraPos - worldPosition);
}

float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

float3 CalculateDirectionalLight(Light directionalLight, VertexToPixel input, float3 surfaceColor, float3 specularColor, float metalness, float roughness)
{
    // Get direction to light from entity
    float3 dirToLight = normalize(-directionalLight.Direction);
    
    // Calculate light
    float diff = DiffusePBR(input.normal, dirToLight);
    float3 F;
    float3 spec = MicrofacetBRDF(input.normal, dirToLight, CalculateViewVector(input.worldPos), roughness, specularColor, F);
    
    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    
    // Combine the final diffuse and specular values for this light
    return (balancedDiff * surfaceColor + spec) * directionalLight.Intensity * directionalLight.Color;
}

float3 CalculatePointLight(Light pointLight, VertexToPixel input, float3 surfaceColor, float3 specularColor, float metalness, float roughness)
{
    // Get direction to light from entity
    float3 dirToLight = normalize(-pointLight.Direction);
    float3 dirToCamera = normalize(cameraPos - input.worldPos);
    
    // Calculate light
    float attenuation = Attenuate(pointLight, input.worldPos);
    
    // Optimization if light isn't visible
    if (attenuation <= 0)
        return 0;
    else
    {
        float diff = DiffusePBR(input.normal, dirToLight);
    
        float3 F;
        float3 spec = MicrofacetBRDF(input.normal, dirToLight, CalculateViewVector(input.worldPos), roughness, specularColor, F);
    
        float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    
        
        // Combine the final diffuse and specular values for this light
        return (balancedDiff * surfaceColor + spec) * pointLight.Intensity * pointLight.Color * attenuation;
    }
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
    float3 albedoColor = pow(Albedo.Sample(BasicSampler, input.uv).rgb, 2.2f);
    float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;
    
    float3 totalLight = 0;
    
    // Normalize normal
    input.normal = normalize(input.normal);
    
    //return input.tangent.rgbb;
    
    // Normal Map
    unpackedNormal = normalize(unpackedNormal); // Don’t forget to normalize!
    
    // Feel free to adjust/simplify this code to fit with your existing shader(s)
    // Simplifications include not re-normalizing the same vector more than once!
    float3 N = normalize(input.normal); // Normalized earlier
    float3 T = normalize(input.tangent); // Must be normalized here or before
    T = normalize(T - N * dot(T, N)); // Gram-Schmidt assumes T&N are normalized!
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    
    // Assumes that input.normal is the normal later in the shader
    input.normal = mul(unpackedNormal, TBN); // Note multiplication order!
    

    
    // Specular color determination -----------------
    // Assume albedo texture is actually holding specular color where metalness == 1
    // Note the use of lerp here - metal is generally 0 or 1, but might be in between
    // because of linear texture sampling, so we lerp the specular color to match
    float3 specularColor = lerp(F0_NON_METAL, albedoColor.rgb, metalness);
    
    for (int i = 0; i < 5; i++)
    {
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIR:
                totalLight += CalculateDirectionalLight(lights[i], input, albedoColor, specularColor, metalness, roughness);
                break;
            
            case LIGHT_TYPE_POINT:
                totalLight += CalculatePointLight(lights[i], input, albedoColor, specularColor, metalness, roughness);
                break;
            
            default:
                break;
        }
    }
    return float4(pow(totalLight, 1.0f / 2.2f), 1);

}
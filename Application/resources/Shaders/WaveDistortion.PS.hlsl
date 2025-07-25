#include "Object3D.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct WaveParams
{
    float gTime;
    float amplitude;
    float frequency;
    float speed;
};

ConstantBuffer<WaveParams> gWaveParams : register(b5);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float2 uv = input.texcoord;
    
    // X•ûŒü‚É”g‚ğ—^‚¦‚éiY•ûŒü‚É—h‚ê‚éj
    uv.y += sin(uv.x * gWaveParams.frequency + gWaveParams.gTime * gWaveParams.speed) * gWaveParams.amplitude;
    
    // Y•ûŒü‚É‚à”g‚ğ—^‚¦‚éiX•ûŒü‚É—h‚ê‚éj
    uv.x += sin(uv.y * gWaveParams.frequency + gWaveParams.gTime * gWaveParams.speed) * gWaveParams.amplitude;
    
    output.color = gTexture.Sample(gSampler, uv);
    
    return output;
}
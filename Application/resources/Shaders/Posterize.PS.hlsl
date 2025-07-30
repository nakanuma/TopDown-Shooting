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

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

// カラーレベルの数
static const int Levels = 4;

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float32_t4 color = gTexture.Sample(gSampler, input.texcoord);
    
    // 各成分をポスタライズ
    float32_t3 posterizeColor = floor(color.rgb * Levels) / (Levels - 1);
    
    output.color = float32_t4(posterizeColor, color.a);
    
    return output;
}
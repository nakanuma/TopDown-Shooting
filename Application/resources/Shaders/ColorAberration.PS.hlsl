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

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // 色ズレ量（倍率）
    float amount = 0.002f;
    
    float32_t2 offsetR = float32_t2(amount, amount);
    float32_t2 offsetG = float32_t2(-amount, -amount);
    float32_t2 offsetB = float32_t2(0.0f, 0.0f);
    
    // 各チャンネルごとに異なる方向へずらす
    float32_t3 color;
    color.r = gTexture.Sample(gSampler, input.texcoord + offsetR).r;
    color.g = gTexture.Sample(gSampler, input.texcoord + offsetG).g;
    color.b = gTexture.Sample(gSampler, input.texcoord + offsetB).b;
    
    output.color = float32_t4(color, 1.0f);

    return output;
}
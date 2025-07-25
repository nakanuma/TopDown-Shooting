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

static const float32_t2 texelSize = float32_t2(1.0f / 1280.0f, 1.0f / 720.0f);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float32_t3 result = float32_t3(0.0f, 0.0f, 0.0f);
    float32_t3 center = gTexture.Sample(gSampler, input.texcoord).rgb;
    
    // シャープフィルターの適用
    result += center * 5.0f;
    result -= gTexture.Sample(gSampler, input.texcoord + float32_t2(-texelSize.x, 0)).rgb;
    result -= gTexture.Sample(gSampler, input.texcoord + float32_t2(texelSize.x, 0)).rgb;
    result -= gTexture.Sample(gSampler, input.texcoord + float32_t2(-texelSize.y, 0)).rgb;
    result -= gTexture.Sample(gSampler, input.texcoord + float32_t2(texelSize.y, 0)).rgb;
    
    // 0~1にクランプ
    result = saturate(result);
    
    output.color = float32_t4(result, 1.0f);
    
    return output;
}
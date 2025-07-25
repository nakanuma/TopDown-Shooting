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
    
    // 隣接ピクセルを取得
    float32_t3 col1 = gTexture.Sample(gSampler, input.texcoord + texelSize * float32_t2(-1.0f, -1.0f)).rgb;
    float32_t3 col2 = gTexture.Sample(gSampler, input.texcoord + texelSize * float32_t2(1.0f, 1.0f)).rgb;
    
    // 差分をとってエンボス効果
    float32_t3 emboss = col1 - col2 + 0.5f;
    
    float gray = (emboss.r + emboss.g + emboss.b) / 3.0f;
    
    output.color = float32_t4(gray, gray, gray, 1.0f);
    
    return output;
}
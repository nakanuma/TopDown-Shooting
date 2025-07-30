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
    
    float32_t4 color = gTexture.Sample(gSampler, input.texcoord);
    float32_t3 original = color.rgb;
    
    // セピア変換行列を使用
    float32_t3 sepia;
    sepia.r = dot(original, float32_t3(0.393, 0.796, 0.189));
    sepia.g = dot(original, float32_t3(0.349, 0.686, 0.168));
    sepia.b = dot(original, float32_t3(0.272, 0.534, 0.131));
    
    // オーバーフロー対策にクランプ
    float32_t3 finalColor = saturate(sepia);
    
    output.color = float32_t4(finalColor, 1.0f);
    
    return output;
}
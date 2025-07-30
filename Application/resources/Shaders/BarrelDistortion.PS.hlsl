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
    
    float2 uv = input.texcoord;
    float2 center = float2(0.5f, 0.5f);
    float2 offset = uv - center;
    
    // 歪ませる強さ（正数ならBarrel, 負数ならPincushion）
    float distortionAmount = 0.5f;
    
    // 距離に応じた歪み
    float r2 = dot(offset, offset);
    float2 distortedUV = center + offset * (1.0f + distortionAmount * r2);
    
    // 範囲外なら黒に（optional）
    if (distortedUV.x < 0.0f || distortedUV.x > 1.0f || distortedUV.y < 0.0f || distortedUV.y > 1.0f)
    {   
        output.color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        output.color = gTexture.Sample(gSampler, distortedUV);
    }
    
    return output;
}
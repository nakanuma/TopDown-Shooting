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

static const float2 screenSize = float2(1280.0f, 720.0f);
static const float blockSize = 8.0f;

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float2 uv = input.texcoord;
   
    // ブロック単位にUVをスナップ
    float2 pixelUV = floor(uv * screenSize / blockSize) * blockSize / screenSize;
    
    // スナップされたUVでサンプリング（ブロック内で同じ色にする）
    float4 color = gTexture.Sample(gSampler, pixelUV);
    
    output.color = color;
    
    return output;
}
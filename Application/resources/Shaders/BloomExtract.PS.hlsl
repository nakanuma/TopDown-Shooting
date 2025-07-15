#include "BloomExtract.hlsli"

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
    
    // テクスチャから色取得
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    // 輝度を計算
    float luminance = dot(textureColor.rgb, float32_t3(0.299, 0.587, 0.114)); // RGB->輝度への変換
    
    // 閾値より暗ければ破棄する
    if (luminance < 0.4f)
    {
        discard;
    }
    
    output.color = textureColor;
    
    return output;
}
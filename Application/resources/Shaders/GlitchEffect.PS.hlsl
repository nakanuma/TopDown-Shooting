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

struct GlitchParams
{
    float gTime;
    float intensity;
    float speed;
    float padding;
};

ConstantBuffer<GlitchParams> gGlitchParams : register(b6);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float2 uv = input.texcoord;
    
    // バンド数（何行にするか）
    float numBands = 10.0f;
    float bandIndex = floor(uv.y * numBands);
    
    // 行ごとの異なるランダムシードを生成
    float seed = bandIndex * 8.2315 + gGlitchParams.gTime * gGlitchParams.speed;
    // 行ごとのランダムノイズ（0~1）
    float bandNoize = frac(sin(seed) * 53672.2563);
    
    // ノイズが閾値を超えた行のみずらしを入れる
    float offsetX = 0.0f;
    if (bandNoize > 0.7f)
    {   
        // 強度とノイズで横方向のずれを作る
        float direction = (bandNoize > 0.85f) ? -1.0f : 1.0f; // 一部逆方向に
        offsetX = direction * gGlitchParams.intensity * 0.03f; // 最大3％画面幅のずれ
    }
    
    float2 uvGlitch = uv + float2(offsetX, 0.0f);
    
    // RGBずらし
    float r = gTexture.Sample(gSampler, uvGlitch + float2(gGlitchParams.intensity * 0.01f, 0.0f)).r;
    float g = gTexture.Sample(gSampler, uvGlitch).g;
    float b = gTexture.Sample(gSampler, uvGlitch - float2(gGlitchParams.intensity * 0.01f, 0.0f)).b;
    
    output.color = float4(r, g, b, 1.0f);
    
    return output;
}
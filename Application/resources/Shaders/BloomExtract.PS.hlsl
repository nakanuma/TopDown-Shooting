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
    
    // テクスチャから色取得
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    // 最大成分ベースでの判定
    float maxComponent = max(max(textureColor.r, textureColor.g), textureColor.b);
    
    // 輝度を計算
    float luminance = dot(textureColor.rgb, float32_t3(0.2126, 0.7152, 0.0722)); // RGB->輝度への変換
    
    // 最大成分と輝度の療法を考慮して色を取得
    float brightness = max(maxComponent, luminance);
    
    // 閾値の設定
    float threshold = 0.8f; // 高輝度のみ抽出
    float solfThreshold = 0.5f; // ソフトな遷移の範囲
    
    // ソフトな閾値処理
    float extractBrightness = max(0.0f, brightness - threshold);
    extractBrightness = extractBrightness / (extractBrightness + solfThreshold);
    
    // 輝度が閾値以下の場合は黒を出力
    output.color = float32_t4(textureColor.rgb * extractBrightness, 1.0f);
    
    return output;
}
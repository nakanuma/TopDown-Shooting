#include "Object3D.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct DamageVignetteParams
{
    float intensity;    /* ダメージの強度 */
    float radius;       /* 赤くなる範囲の半径 */
    float softness;     /* エッジのぼかし具合 */
    float padding;      /* パディング */
};

ConstantBuffer<DamageVignetteParams> gDamageVignetteParams : register(b10);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // 元のシーンカラーを取得
    float4 sceneColor = gTexture.Sample(gSampler, input.texcoord);
    
    // UV座標を中心からの距離に変換
    float2 centerOffset = input.texcoord - float2(0.5f, 0.5f);
    float distanceFromCenter = length(centerOffset);
    
    // ビネットの計算
    float vignette = smoothstep(gDamageVignetteParams.radius, gDamageVignetteParams.radius + gDamageVignetteParams.softness, distanceFromCenter);
    
    // ダメージカラー
    float3 damageColor = float3(1.0f, 0.0f, 0.0f);
    
    // 強度に応じてダメージカラーをブレンド
    float3 finalColor = lerp(sceneColor.rgb, damageColor, vignette * gDamageVignetteParams.intensity);
    
    // 画面を少し暗くする
    finalColor *= 1.0f - (vignette * gDamageVignetteParams.intensity * 0.3f);
    
    output.color = float4(finalColor, sceneColor.a);
    
    return output;
}
#include "ShadowMap.hlsli"

cbuffer ShadowCB : register(b1)
{
    float4x4 World;
    float4x4 LightViewProj;
}

struct VertexShaderInput
{
    float3 position : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    // 1.ワールド座標
    float4 worldPos = mul(float4(input.position, 1.0f), World);
    
    // 2.ライトビュー座標
    float4 viewPos = mul(worldPos, LightViewProj);
    
    // 4. 0~1正規化座標
    float3 projCoords = viewPos.xyz / viewPos.w;
    projCoords.xy = projCoords.xy * 0.5f + 0.5f;
    
    // 最終描画位置
    output.position = viewPos;
    
    return output;
}
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    
    float3 debugWorldPos : TEXCOORD0; // ワールド座標
    float3 debugViewPos : TEXCOORD1; // ライトビュー座標
    float3 debugClipPos : TEXCOORD2; // クリップ座標
    float3 debugProjPos : TEXCOORD3; // 0~1正規化座標
};
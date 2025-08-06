#include "Sprite.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float shininess;
    float ratio;
    float padding;
    float32_t4x4 uvTransform;
    int32_t useCircleMask;
    float32_t3 padding2;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct Camera
{
    float32_t3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b2);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

static const float pi = 3.14159265f;

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t2 uv = transformedUV.xy;
    
    if (gMaterial.useCircleMask != 0)
    {
        // ƒeƒNƒXƒ`ƒƒ’†S‚©‚ç‚Ì‘Š‘ÎˆÊ’u
        float32_t2 delta = uv - float32_t2(0.5f, 0.5f);
    
        // ‹——£‚Å‰~Œ`ƒ}ƒXƒN
        float radius = length(delta);
        if (radius > 0.5f)
        {
            discard;
        }
    
        // Šp“xŒvŽZ
        float angle = atan2(delta.y, delta.x);
        if (angle < 0)
        {
            angle += 2.0f * pi;
        }
        
        // ã•ûŒü‚©‚ç
        angle -= (1.5f * pi);
        if (angle < 0)
        {
            angle += 2.0f * pi;
        }
    
        // •`‰æŠp“xãŒÀ
        float visibleAngle = gMaterial.ratio * 2.0f * pi;
    
        // •`‰æ”ÍˆÍŠO‚È‚ç”jŠü
        if (angle > visibleAngle)
        {
            discard;
        }
    }
    
    float32_t4 textureColor = gTexture.Sample(gSampler, uv);
    
    // texture‚Ìa’l‚ª0.5ˆÈ‰º‚Ì‚Æ‚«‚ÉPixel‚ðŠü‹p
    if (textureColor.a <= 0.5)
    {
        discard;
    }
    // texture‚Ìa’l‚ª0‚Ì‚Æ‚«‚ÉPiel‚ðŠü‹p
    if (textureColor.a == 0.0)
    {
        discard;
    }
    // output.color‚Ìa’l‚ª0‚Ì‚Æ‚«‚ÉPixel‚ðŠü‹p
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    output.color = gMaterial.color * textureColor;
    
    return output;
}
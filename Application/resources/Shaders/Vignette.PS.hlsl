#include "Vignette.hlsli"

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
    
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    // üˆÍ‚ğ0‚ÉA’†S‚É‚È‚é‚Ù‚Ç–¾‚é‚­‚È‚é‚æ‚¤‚ÉŒvZ‚Å’²®
    float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    // correct‚¾‚¯‚ÅŒvZ‚·‚é‚Æ’†S‚ÌÅ‘å’l‚ª0.0625‚ÅˆÃ‚·‚¬‚é‚Ì‚ÅScale‚Å’²®B‚±‚Ì—á‚Å‚Í16”{‚µ‚Ä1‚É‚µ‚Ä‚¢‚é
    float vignette = correct.x * correct.y * 16.0f;
    // ‚Æ‚è‚ ‚¦‚¸0.8æ‚Å‚»‚ê‚Á‚Û‚­
    vignette = saturate(pow(vignette, 0.8f));
    // ŒW”‚Æ‚µ‚ÄæZ
    output.color.rgb *= vignette;
    
    return output;
}
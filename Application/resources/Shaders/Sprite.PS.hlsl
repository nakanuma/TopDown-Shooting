#include "Sprite.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
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

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition); // Camera�ւ̕������Z�o
    float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal)); // ���ˌ��̔��˃x�N�g��
    
    // texture��a�l��0.5�ȉ��̂Ƃ���Pixel�����p
    if (textureColor.a <= 0.5)
    {
        discard;
    }
    // texture��a�l��0�̂Ƃ���Piel�����p
    if (textureColor.a == 0.0)
    {
        discard;
    }
    // output.colo��a�l��0�̂Ƃ���Pixel�����p
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    if (gMaterial.enableLighting != 0)
    { // Lighting����ꍇ  
        ///
        /// DirectionalLight
        ///
        
        // �g�U����
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f); // half lambert
        
        float32_t3 diffuseDirectionalLight =
        gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        
        // ���ʔ���
        float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
        float NdotH = dot(normalize(input.normal), halfVector);
        float specularPow = pow(saturate(NdotH), gMaterial.shininess); // ���ˋ��x
        
        float32_t3 specularDirectionalLight =
        gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        
        
        
        // ���C�e�B���O�e�X�g�p
        // �g�U����+���ʔ���
        //output.color.rgb = diffuseDirectionalLight + specularDirectionalLight;
        
        // ��{�I�ɂ͂�������K�p
        output.color.rgb = diffuseDirectionalLight;
        
        
        // �A���t�@�͍��܂Œʂ�
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    return output;
}
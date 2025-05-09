#include "Skybox.hlsli"

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

struct PointLight
{
    float32_t4 color;
    float32_t3 position;
    float intensity;
    float radius;
    float decay;
};
ConstantBuffer<PointLight> gPointLight : register(b3);

struct SpotLight
{
    float32_t4 color; // ���C�g�̐F
    float32_t3 position; // ���C�g�̈ʒu
    float32_t intensity; // �P�x
    float32_t3 direction; // �X�|�b�g���C�g�̕���
    float32_t distance; // ���C�g�̓͂��ő勗��
    float32_t decay; // ������
    float32_t cosAngle; // �X�|�b�g���C�g�̗]��
    float32_t cosFalloffStart; // Falloff�J�n�̊p�x
    uint32_t isActive;
};

static const int kMaxLight = 64;

struct SpotLights
{
    SpotLight spotLights[kMaxLight];
};

ConstantBuffer<SpotLights> gSpotLight : register(b4);

TextureCube<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    output.color = textureColor * gMaterial.color;
    return output;
}
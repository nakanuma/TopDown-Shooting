#include "RadialBlur.hlsli"

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
    const float32_t2 kCenter = float32_t2(0.5f, 0.5f); // ���S�_�B��������ɕ��ˏ�Ƀu���[��������
    const int32_t kNumSamples = 10; // �T���v�����O���B�����قǊ��炩�����d��
    const float32_t kBlurWidth = 0.01f; // �ڂ����̕��B�傫���قǑ傫��
    // ���S���猻�݂�UV�ɑ΂��Ă̕������v�Z�B
    // ���i�����Ƃ����ΒP�ʃx�N�g�������A�����ł͂����Đ��K�������A�����قǂ�艓�����T���v�����O����
    float32_t2 direction = input.texcoord - kCenter;
    float32_t3 outputColor = float32_t3(0.0f, 0.0f, 0.0f);
    for (int32_t sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex)
    {
        // ���݂�uv���炳���قǌv�Z���������ɃT���v�����O�_��i�߂Ȃ���T���v�����O���Ă���
        float32_t2 texcoord = input.texcoord + direction * kBlurWidth * float32_t(sampleIndex);
        outputColor.rgb += gTexture.Sample(gSampler, texcoord).rgb;
    }
    // ���ω�����
    outputColor.rgb *= rcp(kNumSamples);
    
    PixelShaderOutput output;
    output.color.rgb = outputColor;
    output.color.a = 1.0f;
    return output;
}
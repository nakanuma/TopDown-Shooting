#include "GaussianFilter.hlsli"

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

static const float32_t2 kIndex3x3[3][3] =
{
    {{ -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f }},
    {{ -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }},
    {{ -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }}
};

static const float32_t PI = 3.14159265;

float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // uvStepの計算
    uint32_t width, height;
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
    
    // Kernelを求める。weightは後で使う
    float32_t weight = 0.0f;
    float32_t sigma = 2.0f;
    float32_t kernel3x3[3][3];
    for (int32_t x1 = 0; x1 < 3; ++x1)
    {
        for (int32_t y1 = 0; y1 < 3; ++y1)
        {
            kernel3x3[x1][y1] = gauss(kIndex3x3[x1][y1].x, kIndex3x3[x1][y1].y, sigma);
            weight += kernel3x3[x1][y1];
            
        }
    }
    
    // フィルターの適用
    float32_t3 result = float32_t3(0.0f, 0.0f, 0.0f);
    
    for (int32_t x2 = 0; x2 < 3; ++x2)
    {
        for (int32_t y2 = 0; y2 < 3; ++y2)
        {
            float32_t2 offset = kIndex3x3[x2][y2] * uvStepSize;
            float32_t2 texcoord = input.texcoord + offset;
            float32_t3 sampleColor = gTexture.Sample(gSampler, texcoord).rgb;
            result += sampleColor * kernel3x3[x2][y2];
        }
    }
    
    // 合計が1になるように正規化
    result *= rcp(weight);
    
    output.color = float4(result, 1.0f);
    return output;
}
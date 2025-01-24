#include"Object3d.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
};

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};

struct Camera
{
    float32_t3 worldPosition;
};

struct PointLight
{
    float32_t4 color;
    float32_t3 position;
    float intensity;
};

struct SpotLight
{
    float32_t4 color;
    float32_t3 position;
    float32_t intensity;
    float32_t3 direction;
    float32_t distance;
    float32_t decay;
    float32_t cosAngle;
    float32_t cosFalloffStart;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);
ConstantBuffer<SpotLight> gSpotLight : register(b4);

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

    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
    float NDotH = dot(normalize(input.normal), halfVector);
    float speclarPow = pow(saturate(NDotH), gMaterial.shininess);

    if (gMaterial.enableLighting != 0)
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float32_t3 diffuseDirectionalLight = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        float32_t3 specularDirectionalLight = gDirectionalLight.color.rgb * gDirectionalLight.intensity * speclarPow * float32_t3(1.0f, 1.0f, 1.0f);

        float32_t distancePoint = length(gPointLight.position - input.worldPosition);
        float32_t factorPoint = 1.0f / (distancePoint * distancePoint + 0.001f);
        float32_t3 pointLightDirection = normalize(gPointLight.position - input.worldPosition);
        float NdotLPoint = dot(normalize(input.normal), pointLightDirection);
        float cosPoint = pow(NdotLPoint * 0.5f + 0.5f, 2.0f);
        float32_t3 diffusePointLight = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cosPoint * gPointLight.intensity * factorPoint;
        float32_t3 specularPointLight = gPointLight.color.rgb * gPointLight.intensity * factorPoint * pow(saturate(dot(normalize(input.normal), normalize(pointLightDirection + toEye))), gMaterial.shininess);

        float32_t3 spotLightDirection = normalize(gSpotLight.position - input.worldPosition);
        float cosAngle = dot(spotLightDirection, gSpotLight.direction);
        float32_t falloffFactor = saturate((cosAngle - gSpotLight.cosFalloffStart) / (gSpotLight.cosAngle - gSpotLight.cosFalloffStart));
        float32_t distanceSpot = length(gSpotLight.position - input.worldPosition);
        float32_t factorSpot = 1.0f / (distanceSpot * distanceSpot + 0.001f);
        float NdotLSpot = dot(normalize(input.normal), spotLightDirection);
        float cosSpot = pow(NdotLSpot * 0.5f + 0.5f, 2.0f);
        float32_t3 diffuseSpotLight = gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * cosSpot * gSpotLight.intensity * factorSpot * falloffFactor;
        float32_t3 specularSpotLight = gSpotLight.color.rgb * gSpotLight.intensity * factorSpot * falloffFactor * pow(saturate(dot(normalize(input.normal), normalize(spotLightDirection + toEye))), gMaterial.shininess);

        output.color.rgb = diffuseDirectionalLight + specularDirectionalLight + diffusePointLight + specularPointLight + diffuseSpotLight + specularSpotLight;

        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }

    return output;
}

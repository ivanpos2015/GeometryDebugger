Texture2D shaderTexture;
SamplerState SampleType;

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float4 color : COLOR;
};

float4 FontPixelShader(PixelInputType input) : SV_TARGET
{
    float4 color;
	
    // Sample the texture pixel at this location
    color = shaderTexture.Sample(SampleType, input.tex);
	
    // If the color is black on the texture then treat this pixel as transparent
    if(color.r == 0.0f)
    {
        color.a = 0.0f;
    }
	
    // If the color is other than black on the texture then this is a pixel in the font so draw it using the font pixel color
    else
    {
        color.a = 1.0f;
        color = color * input.color;
    }

    return color;
}
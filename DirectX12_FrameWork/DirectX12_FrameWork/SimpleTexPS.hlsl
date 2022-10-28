struct VSOutput
{
    float4  Position : SV_POSITION; //!< 位置座標です.
    float2  TexCoord : TEXCOORD;    //!< テクスチャ座標です.
};

///////////////////////////////////////////////////////////////////////////////
// PSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct PSOutput
{
    float4  Color : SV_TARGET0;     //!< 出力カラーです.
};

//-----------------------------------------------------------------------------
// Textures and Samplers
//-----------------------------------------------------------------------------
SamplerState ColorSmp : register(s0);
Texture2D    ColorMap : register(t0);

//-----------------------------------------------------------------------------
//      ピクセルシェーダのメインエントリーポイントです.
//-----------------------------------------------------------------------------
PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput)0;

    output.Color = ColorMap.Sample(ColorSmp, input.TexCoord);
    //output.Color = float4(input.TexCoord.xy,1.0f,1.0f);


    return output;
}
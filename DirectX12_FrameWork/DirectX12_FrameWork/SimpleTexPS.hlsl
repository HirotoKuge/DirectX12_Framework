struct VSOutput
{
    float4  Position : SV_POSITION; //!< �ʒu���W�ł�.
    float2  TexCoord : TEXCOORD;    //!< �e�N�X�`�����W�ł�.
};

///////////////////////////////////////////////////////////////////////////////
// PSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct PSOutput
{
    float4  Color : SV_TARGET0;     //!< �o�̓J���[�ł�.
};

//-----------------------------------------------------------------------------
// Textures and Samplers
//-----------------------------------------------------------------------------
SamplerState ColorSmp : register(s0);
Texture2D    ColorMap : register(t0);

//-----------------------------------------------------------------------------
//      �s�N�Z���V�F�[�_�̃��C���G���g���[�|�C���g�ł�.
//-----------------------------------------------------------------------------
PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput)0;

    output.Color = ColorMap.Sample(ColorSmp, input.TexCoord);
    //output.Color = float4(input.TexCoord.xy,1.0f,1.0f);


    return output;
}
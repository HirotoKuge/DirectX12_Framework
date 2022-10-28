#pragma once
#pragma once

#include <DirectXMath.h>
#include <d3dx12.h>
#include "ComPtr.h"

struct Vertex{
    DirectX::XMFLOAT3 Position;     // �ʒu���W
    DirectX::XMFLOAT3 Normal;       // �@��
    DirectX::XMFLOAT2 UV;           // uv���W
    DirectX::XMFLOAT3 Tangent;      // �ڋ��
    DirectX::XMFLOAT4 Color;        // ���_�F
    static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
    static const int InputElementCount = 5;
    static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct alignas(256) Transform{
    DirectX::XMMATRIX World;    // ���[���h�s��
    DirectX::XMMATRIX View;     // �r���[�s��
    DirectX::XMMATRIX Proj;     // ���e�s��
};

class MeshVertex {
public:
    DirectX::XMFLOAT3   Position;
    DirectX::XMFLOAT3   Normal;
    DirectX::XMFLOAT2   TexCoord;
    DirectX::XMFLOAT3   Tangent;
    
    MeshVertex() = default;
    
    MeshVertex(
        DirectX::XMFLOAT3 const& position,
        DirectX::XMFLOAT3 const& normal,
        DirectX::XMFLOAT2 const& texcoord,
        DirectX::XMFLOAT3 const& tangent)
        : Position(position)
        , Normal(normal)
        , TexCoord(texcoord)
        , Tangent(tangent)
    { /* DO_NOTHING */
    }
    
    static const D3D12_INPUT_LAYOUT_DESC InputLayout;
    
private:
    static const int InputElementCount = 4;
    static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct ResMesh {
    std::vector<MeshVertex> Vertices;   // ���_�f�[�^
    std::vector<uint32_t> Indices;      // �C���f�b�N�X�f�[�^
    uint32_t matetialID;                // �}�e���A���ԍ�
};

struct ResMaterial{
    DirectX::XMFLOAT3   Diffuse;            // �g�U���ː���
    DirectX::XMFLOAT3   Specular;           // ���ʔ��ː���
    float               Alpha;              // ���ߐ���
    float               Shininess;          // ���ʔ��ˋ��x
    std::wstring        DiffuseMapPath;     // �f�B�t���[�Y�}�b�v�t�@�C���p�X
    std::wstring        SpecularMapPath;    // �X�y�L�����[�}�b�v�t�@�C���p�X
    std::wstring        ShininessMapPath;   // �V���C�l�X�}�b�v�t�@�C���p�X
    std::wstring        NormalMapPath;      // �@���}�b�v�t�@�C���p�X
};
#pragma once
#pragma once

#include <DirectXMath.h>
#include <d3dx12.h>
#include "ComPtr.h"

struct Vertex{
    DirectX::XMFLOAT3 Position;     // 位置座標
    DirectX::XMFLOAT3 Normal;       // 法線
    DirectX::XMFLOAT2 UV;           // uv座標
    DirectX::XMFLOAT3 Tangent;      // 接空間
    DirectX::XMFLOAT4 Color;        // 頂点色
    static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
    static const int InputElementCount = 5;
    static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct alignas(256) Transform{
    DirectX::XMMATRIX World;    // ワールド行列
    DirectX::XMMATRIX View;     // ビュー行列
    DirectX::XMMATRIX Proj;     // 投影行列
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
    std::vector<MeshVertex> Vertices;   // 頂点データ
    std::vector<uint32_t> Indices;      // インデックスデータ
    uint32_t matetialID;                // マテリアル番号
};

struct ResMaterial{
    DirectX::XMFLOAT3   Diffuse;            // 拡散反射成分
    DirectX::XMFLOAT3   Specular;           // 鏡面反射成分
    float               Alpha;              // 透過成分
    float               Shininess;          // 鏡面反射強度
    std::wstring        DiffuseMapPath;     // ディフューズマップファイルパス
    std::wstring        SpecularMapPath;    // スペキュラーマップファイルパス
    std::wstring        ShininessMapPath;   // シャイネスマップファイルパス
    std::wstring        NormalMapPath;      // 法線マップファイルパス
};
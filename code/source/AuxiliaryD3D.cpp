#include "AuxiliaryD3D.h"


namespace Rendering
{
    // ����һ������Ĭ�϶����ѳ�ʼ���Ļ�����
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
        ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
        const void* initData,                                 // ����������
        UINT64 byteSize,                                      // �������ֽ���
        Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer) // �򻺳����ϴ����ݵ��ϴ�������
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer; // ������������Դ

        // ����ʵ�ʵ�Ĭ�϶��еĻ�������Դ
        Exception::CheckResult(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

        // ���������ϴ����еĻ�������Դ
        Exception::CheckResult(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

        // ������Ҫ���Ƶ�Ĭ�϶��л�����������
        D3D12_SUBRESOURCE_DATA subResourceData = {};
        subResourceData.pData = initData;
        subResourceData.RowPitch = byteSize;
        subResourceData.SlicePitch = subResourceData.RowPitch;

        // ֪ͨ��������ȥͬ������Դ�Ķ������
        cmdList->ResourceBarrier(
            1,
            &CD3DX12_RESOURCE_BARRIER::Transition(
                defaultBuffer.Get(),
                D3D12_RESOURCE_STATE_COMMON,
                D3D12_RESOURCE_STATE_COPY_DEST));

        // ���ø��������Ƚ����ݴ�CPU�˵��ڴ渴�Ƶ��ϴ����У��ٰ��ϴ����е����ݸ��Ƶ�Ĭ�϶ѵĻ�������
        UpdateSubresources(
            cmdList,
            defaultBuffer.Get(), uploadBuffer.Get(),
            0, 0, 1, &subResourceData);

        // ͬ��
        cmdList->ResourceBarrier(
            1,
            &CD3DX12_RESOURCE_BARRIER::Transition(
                defaultBuffer.Get(),
                D3D12_RESOURCE_STATE_COPY_DEST,
                D3D12_RESOURCE_STATE_GENERIC_READ));

        // ע��˺������غ󲻿���������uploadBuffer��������ΪUpdataSubresource������ʹ��cmdList
        // ��GPU�ύ�����GPU��uploadBuffer�е����ݸ��Ƶ�Ĭ�϶ѵĻ�����defaultBuffer��
        // �������uploadBufferǰ����ȷ��GPU�ĸ���ָ���Ѿ����
        return defaultBuffer;
    }

    // ���������ڴ�Ҫ�����С������������С
    UINT CalcConstantBufferByteSize(UINT byteSize)
    {
        return (byteSize + 255) & ~255;
    }

    // ������ɫ���ļ������ر������ֽ���
    Microsoft::WRL::ComPtr<ID3DBlob>
        CompileShader(
            const UString& filename,
            const D3D_SHADER_MACRO* defines,
            const UString& entrypoint,
            const UString& target)
    {
        UINT compileFlags = 0;
        // ���ڵ���ģʽʱ������ɫ�������Ż������õ���ģʽ������ɫ��
#if defined(DEBUG) || defined(_DEBUG)
        compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        HRESULT hr = S_OK;

        Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errors;
        hr = D3DCompileFromFile(filename.GetWstring().c_str(), defines,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            entrypoint.GetString().c_str(), target.GetString().c_str(), compileFlags, 0, &byteCode, &errors);

        // �������ʱ
        if (errors != nullptr)
            std::cout << (char*)errors->GetBufferPointer() << std::endl;

        Exception::CheckResult(hr);

        return byteCode;
    }

    // ���Ѿ�����õ���ɫ��.cso�ֽ����ļ����ص�Ӧ�ó������
    Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const UString& filename)
    {
        std::ifstream fin(filename.GetWstring(), std::ios::binary);

        fin.seekg(0, std::ios_base::end);
        std::ifstream::pos_type size = (int)fin.tellg();
        fin.seekg(0, std::ios_base::beg);

        Microsoft::WRL::ComPtr<ID3DBlob> blob;
        Exception::CheckResult(D3DCreateBlob(size, blob.GetAddressOf()));

        fin.read((char*)blob->GetBufferPointer(), size);
        fin.close();

        return blob;
    }

    DirectX::XMFLOAT4X4 Identity4X4()
    {
        return { 1.0f,0.0f,0.0f,0.0f,
                0.0f,1.0f,0.0f,0.0f,
                0.0f,0.0f,1.0f,0.0f,
                0.0f,0.0f,0.0f,1.0f };
    }
}
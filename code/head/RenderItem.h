#pragma once

namespace Rendering
{
	class RenderItem
	{
	public:
		RenderItem()
		{
			for (int i = 0; i < 7; ++i)
			{
				textIndex[i] = -1;
			}
			materialIndex = -1;
		}

		inline void SetTextureIndex(const PBR_Material& material)
		{
			for (int i = 0; i < 7; ++i)
			{
				if (i < 3)
					textIndex[i] = material.mapIndex[i];
				else
					textIndex[i] = material.mapIndex[i + 1];
			}
		}

		inline void DrawItem(ID3D12GraphicsCommandList* commandList,
			D3D12_PRIMITIVE_TOPOLOGY primitiveType, D3D12_GPU_VIRTUAL_ADDRESS worldCB)
		{
			commandList->IASetVertexBuffers(0, 1, &vbv);
			commandList->IASetIndexBuffer(&ibv);
			commandList->IASetPrimitiveTopology(primitiveType);

			commandList->SetGraphicsRootConstantBufferView(0, worldCB);

			commandList->DrawIndexedInstanced(
				indexNum, 1, indexStartLocation, vertexStartLocation, 0);
		}

		inline void DrawItem(ID3D12GraphicsCommandList* commandList,
			UINT cbvSrvUavDescriptorSize, D3D12_PRIMITIVE_TOPOLOGY primitiveType,
			D3D12_GPU_VIRTUAL_ADDRESS worldCB, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap,
			D3D12_GPU_VIRTUAL_ADDRESS materialCB, bool suportAnimation)
		{
			commandList->IASetVertexBuffers(0, 1, &vbv);
			commandList->IASetIndexBuffer(&ibv);
			commandList->IASetPrimitiveTopology(primitiveType);

			commandList->SetGraphicsRootConstantBufferView(0, worldCB);
			commandList->SetGraphicsRootConstantBufferView(1, materialCB);

			if (srvDescriptorHeap)
			{
				if (suportAnimation)
				{
					for (int i = 0; i < 7; ++i)
					{
						if (textIndex[i] != -1)
							commandList->SetGraphicsRootDescriptorTable(i + 4,
								GetIndexHandle(textIndex[i], cbvSrvUavDescriptorSize, srvDescriptorHeap));
					}
				}
				else
				{
					for (int i = 0; i < 7; ++i)
					{
						if (textIndex[i] != -1)
							commandList->SetGraphicsRootDescriptorTable(i + 3,
								GetIndexHandle(textIndex[i], cbvSrvUavDescriptorSize, srvDescriptorHeap));
					}
				}
			}

			commandList->DrawIndexedInstanced(
				indexNum, 1, indexStartLocation, vertexStartLocation, 0);
		}

		D3D12_VERTEX_BUFFER_VIEW vbv;
		D3D12_INDEX_BUFFER_VIEW ibv;

		int indexNum;
		int indexStartLocation;
		int vertexStartLocation;

		int textIndex[7];
		int materialIndex;

		DirectX::XMFLOAT4X4 transform;
	};
}
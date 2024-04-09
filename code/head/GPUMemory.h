#pragma once

#include "Model.h"
#include "RenderItemWrap.h"

namespace Rendering
{
	// manager all GPU resource
	class GPUMemory
	{
	public:
		GPUMemory() {};

		// Initialize the required GPU resources based on the model
		void SetModel(Model<Vertex_G>& model)
		{
			if (!model.CheckValid())
				Exception::ThrowException(LEVEL_Error, "model is no valid");

			// geometry info
			itemNumber = model.GetMeshs().size();
			geoResources.SetGeoDescs(model.GetGeoDesc());

			// texture info
			textNumber = model.GetTextFilePaths().size();
			textureResourceWrap.SetTextureFilePaths(model.GetTextFilePaths());

			// materi info
			materialNumber = model.GetMaterialManager().GetMaterials().size();
			materials = &model.GetMaterialManager().GetMaterials();

			suportAnimation = false;

			// generate render item wrap
			renderItemWrap.renderItems.resize(itemNumber);
			for (int i = 0; i < itemNumber; ++i)
			{
				renderItemWrap.renderItems[i].materialIndex = model.GetMeshs()[i].GetPBRMaterialIndex();
				renderItemWrap.renderItems[i].transform = model.GetMeshs()[i].GetTransform();
				renderItemWrap.renderItems[i].SetTextureIndex(
					model.GetMaterialManager().GetMaterials()[renderItemWrap.renderItems[i].materialIndex]);
			}
		}

		void SetModel(Model<Vertex_GB>& model)
		{
			itemNumber = model.GetMeshs().size();
			geoResources.SetGeoDescs(model.GetGeoDesc());

			textNumber = model.GetTextFilePaths().size();
			textureResourceWrap.SetTextureFilePaths(model.GetTextFilePaths());

			materialNumber = model.GetMaterialManager().GetMaterials().size();
			materials = &model.GetMaterialManager().GetMaterials();

			suportAnimation = true;
			animationsP = &model.GetAnimations();

			renderItemWrap.renderItems.resize(itemNumber);
			for (int i = 0; i < itemNumber; ++i)
			{
				renderItemWrap.renderItems[i].materialIndex = model.GetMeshs()[i].GetPBRMaterialIndex();
				renderItemWrap.renderItems[i].transform = model.GetMeshs()[i].GetTransform();
				renderItemWrap.renderItems[i].SetTextureIndex(
					model.GetMaterialManager().GetMaterials()[renderItemWrap.renderItems[i].materialIndex]);
			}
		}

		void CreateResource(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
			UINT cbvSrvUavDescriptorsize, int frameNum = 1, int minMipLevels = 0, int maxMipLevels = -1)
		{
			if (itemNumber <= 0 || isUpload)
				Exception::ThrowException(LEVEL_Error, "error");

			if (itemNumber > 0)
			{
				geoResources.UploadVIDataToGPU(device, cmdList);
				worldCB = ConstBufferManager<WorldMatrix, WorldTransform>(itemNumber, true, true, frameNum);
				worldCB.CreateBufferResource(device);

				WorldMatrix value;
				for (int i = 0; i < itemNumber; ++i)
				{
					value.SetWorld(renderItemWrap.renderItems[i].transform);
					worldCB.InitValue(value.GetData(), i);
				}
				worldCB.EndInit();
			}

			if (textNumber > 0)
			{
				textureResourceWrap.UploadToGPU(device, cmdList);
				textureResourceWrap.CreateSrvResourceView(device, cbvSrvUavDescriptorsize, minMipLevels, maxMipLevels);
			}
			
			if (materialNumber > 0)
			{
				materialCB = ConstBufferManager<PBR_Material_Data, PBR_Material_Data>(materialNumber, false, false, 1);
				materialCB.CreateBufferResource(device);

				for (int i = 0; i < materialNumber; ++i)
				{
					materialCB.InitValue((*materials)[i].data, i);
				}
				materialCB.EndInit();
			}

			if (suportAnimation)
			{
				finalTransformCB = ConstBufferManager<FinalTransforms, FinalTransforms>(1, true, false, frameNum);
				finalTransformCB.CreateBufferResource(device);
				FinalTransforms value;
				finalTransformCB.InitValue(value);
				finalTransformCB.EndInit();
			}

			isUpload = true;

			BuildRenderItemWrap();
		}

		void FreeUploadSpace()
		{
			if (!isUpload)
				Exception::ThrowException(LEVEL_Error, "error");

			if (itemNumber > 0)
				geoResources.FreeUploadAndNewSpace();
			if (textNumber > 0)
				textureResourceWrap.FreeUploadSpace();
		}

		void Reset(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
			UINT cbvSrvUavDescriptorsize, int frameNum = 1, int minMipLevels = 0, int maxMipLevels = -1)
		{
			if (!isUpload)
				Exception::ThrowException(LEVEL_Error, "error");

			worldCB.Release();

			if (textNumber > 0)
			{
				textureResourceWrap.FreeResource();
			}

			if (materialNumber > 0)
			{
				materialCB.Release();
			}

			if (suportAnimation)
			{
				finalTransformCB.Release();
			}

			isUpload = false;

			if (itemNumber > 0)
			{
				worldCB = ConstBufferManager<WorldMatrix, WorldTransform>(itemNumber, true, true, frameNum);
				worldCB.CreateBufferResource(device);

				WorldMatrix value;
				for (int i = 0; i < itemNumber; ++i)
				{
					worldCB.InitValue(value, i);
				}
				worldCB.EndInit();
			}

			if (textNumber > 0)
			{
				textureResourceWrap.UploadToGPU(device, cmdList);
				textureResourceWrap.CreateSrvResourceView(device, cbvSrvUavDescriptorsize, minMipLevels, maxMipLevels);
			}

			if (materialNumber > 0)
			{
				materialCB = ConstBufferManager<PBR_Material_Data, PBR_Material_Data>(materialNumber, false, false, 1);
				materialCB.CreateBufferResource(device);

				for (int i = 0; i < materialNumber; ++i)
				{
					materialCB.InitValue((*materials)[i].data, i);
				}
				materialCB.EndInit();
			}

			if (suportAnimation)
			{
				finalTransformCB = ConstBufferManager<FinalTransforms, FinalTransforms>(1, true, false, frameNum);
				finalTransformCB.CreateBufferResource(device);
				FinalTransforms value;
				finalTransformCB.InitValue(value);
				finalTransformCB.EndInit();
			}

			isUpload = true;
		}

		void FreeResource()
		{
			if (!isUpload)
				Exception::ThrowException(LEVEL_Error, "error");

			geoResources.Release();
			worldCB.Release();

			if (textNumber > 0)
			{
				textureResourceWrap.FreeResource();
			}

			if (materialNumber > 0)
			{
				materialCB.Release();
			}

			if (suportAnimation)
			{
				finalTransformCB.Release();
			}

			isUpload = false;
		}

		void BuildRenderItemWrap()
		{
			if (!isUpload)
				Exception::ThrowException(LEVEL_Error, "error");

			if (textNumber > 0)
			{
				renderItemWrap.suportTexture = true;
				renderItemWrap.srvDescriptorHeap = textureResourceWrap.GetSrvDescriptorHeap();
			}
			else
			{
				renderItemWrap.suportTexture = false;
				renderItemWrap.srvDescriptorHeap = nullptr;
			}

			renderItemWrap.primitiveType = primitiveType;
			renderItemWrap.worldCB = &worldCB;
			renderItemWrap.materialCBP = &materialCB;

			if (suportAnimation)
			{
				renderItemWrap.suportAnimation = true;
				renderItemWrap.animationsP = animationsP;
				renderItemWrap.finalTransformsP = &finalTransformCB;
			}

			for (int i = 0; i < itemNumber; ++i)
			{
				renderItemWrap.renderItems[i].vbv = geoResources.GetGeoDescs()[i].GetVertexView();
				renderItemWrap.renderItems[i].ibv = geoResources.GetGeoDescs()[i].GetIndexView();
				
				renderItemWrap.renderItems[i].indexNum = geoResources.GetGeoDescs()[i].GetIndexCount();
				renderItemWrap.renderItems[i].indexStartLocation = geoResources.GetGeoDescs()[i].GetStartIndexLocation();
				renderItemWrap.renderItems[i].vertexStartLocation = geoResources.GetGeoDescs()[i].GetBaseVertexLocation();
			}

			renderItemWrap.memory = this;
		}

		RenderItemWrap& GetRenderItemWrap()
		{
			return renderItemWrap;
		}

	private:
		bool isUpload = false;

		int itemNumber = 0;
		GeometryResource geoResources;
		ConstBufferManager<WorldMatrix, WorldTransform> worldCB;

		int textNumber = 0;
		TextureResourceWrap textureResourceWrap;

		bool suportAnimation = false;
		std::vector<Animation>* animationsP;
		ConstBufferManager<FinalTransforms, FinalTransforms> finalTransformCB;

		int materialNumber = 0;
		const std::vector<PBR_Material>* materials;
		ConstBufferManager<PBR_Material_Data, PBR_Material_Data> materialCB;

		D3D12_PRIMITIVE_TOPOLOGY primitiveType;
		RenderItemWrap renderItemWrap;
	};
}
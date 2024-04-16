#pragma once

#include "Model.h"
#include "GPUMemory.h"
#include "RenderItemWrap.h"

namespace Rendering
{
	template<typename VertexType>
	class MemoryWrop
	{
	public:
		MemoryWrop() {}

		Model<VertexType>& GetModel()
		{
			return model;
		}

		void LoadModelToCPU(UString path,const aiScene* scene,UString filePath)
		{
			Model<VertexType>::LoadModel(path, scene, model);
		}

		void LoadModelToCPU(Model<VertexType>& _model)
		{
			model = std::move(_model);
		}

		RenderItemWrap* LoadModelToGPU(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
			UINT cbvSrvUavDescriptorsize, int frameNum = 1, int minMipLevels = 0, int maxMipLevels = -1)
		{
			gpuMemory.SetModel(model);

			gpuMemory.CreateResource(device, cmdList, cbvSrvUavDescriptorsize, frameNum, minMipLevels, maxMipLevels);
			gpuMemory.BuildRenderItemWrap();

			return gpuMemory.GetRenderItemWrap();
		}

		RenderItemWrap* GetRenderItemWrap()
		{
			return gpuMemory.GetRenderItemWrap();
		}

		void FreeUploadSpace()
		{
			gpuMemory.FreeUploadSpace();
		}

		GPUMemory& GetGPUMemory()
		{
			return gpuMemory;
		}

	private:
		Model<VertexType> model;
		GPUMemory gpuMemory;
	};


	class MemoryManage
	{
	public:
		MemoryManage(int maxModelNum = 0)
		{
			memorys_g.reserve(maxModelNum);
			memorys_gb.reserve(maxModelNum);
		}

		std::vector<MemoryWrop<Vertex_G>>& GetMemoryG()
		{
			return memorys_g;
		}

		std::vector<MemoryWrop<Vertex_GB>>& GetMemoryGB()
		{
			return memorys_gb;
		}

	private:
		std::vector<MemoryWrop<Vertex_G>> memorys_g;
		std::vector<MemoryWrop<Vertex_GB>> memorys_gb;
	};
}
#pragma once

#include "AuxiliaryD3D.h"
#include "FrameRsource.h"

namespace Rendering
{
	template<typename cbClass>
	class InitItem
	{
	public:
		InitItem(int _cbIndex, cbClass _value) :cbIndex(_cbIndex), value(_value) {}

		int cbIndex;
		cbClass value;
	};

	// manage constant buffers containing multiple objects and frame resource
	template<typename cbClass, typename dataClass>
	class ConstBufferManager
	{
	public:
		/*
			if enable multiple frame resource, set frameNum > 1.
			else set frameNum = 1.
		*/
		ConstBufferManager(int _cbNum = 1, bool _allowChange = true, bool _haveData = true, int _frameNum = 1)
			:cbNum(_cbNum), frameNum(_frameNum), haveData(_haveData), allowChange(_allowChange)
		{
		}

		/*
			create flow:
				createBufferResource -> use、reset -> Release
		*/
		void CreateBufferResource(ID3D12Device* device)
		{
			if (cbNum <= 0 || frameNum <= 0)
				Exception::ThrowException(LEVEL_Warning, "Num == 0");
			if (!allowChange && frameNum != 1)
				Exception::ThrowException(LEVEL_Error, "frameNum != 1");

			if (haveData)
				data.resize(cbNum);

			uploadCB = std::make_shared<UploadBuffer<cbClass>>(device, cbNum * frameNum, true);
		}

		void Release()
		{
			if (uploadCB == nullptr)
				Exception::ThrowException(LEVEL_Error, "not upload");

			uploadCB->~UploadBuffer();
			uploadCB = nullptr;
		}

		void Reset(ID3D12Device* device, int _cbNum, bool _allowChange, bool _haveData, int _frameNum)
		{
			Release();

			cbNum = _cbNum;
			allowChange = _allowChange;
			frameNum = _frameNum;
			haveData = _haveData;

			CreateBufferResource(device);
		}

		void InitValue(cbClass value, int cbIndex = 0)
		{
			if (uploadCB == nullptr)
				Exception::ThrowException(LEVEL_Error, "not upload");
			if (isInit)
				Exception::ThrowException(LEVEL_Error, "isInit = true");

			initItems.emplace_back(cbIndex, value);
		}

		void EndInit()
		{
			for (int i = 0; i < initItems.size(); ++i)
			{
				for (int j = 0; j < frameNum; ++j)
				{
					int index = GetFrameIndexCB(initItems[i].cbIndex, j);
					uploadCB->CopyData(index,initItems[i].value);
				}
			}
			isInit = true;
		}

		/*
			use flow:
				haveData: getData,change,getCBAddress -> rendering
				noData: change,getCBAddress -> rendering
		*/
		dataClass& GetData(int cbIndex = 0)
		{
			if (cbIndex >= data.size() || !haveData)
				Exception::ThrowException(LEVEL_Error, "index out of ranage");
			if (!allowChange)
				Exception::ThrowException(LEVEL_Error, "allow change = false");
			return data[cbIndex];
		}

		void Change(FrameIndex frameIndex, cbClass value, int cbIndex = 0)
		{
			if (uploadCB == nullptr)
				Exception::ThrowException(LEVEL_Error, "not upload");
			if (!allowChange)
				Exception::ThrowException(LEVEL_Error, "allow change = false");
			uploadCB->CopyData(GetFrameIndexCB(cbIndex, frameIndex.GetData()), value);
		}

		D3D12_GPU_VIRTUAL_ADDRESS GetCBAddress(int cbIndex = 0)
		{
			if (uploadCB == nullptr)
				Exception::ThrowException(LEVEL_Error, "not upload");
			if (allowChange)
				Exception::ThrowException(LEVEL_Error, "allowChange need false");

			return uploadCB->GetIndexBufferAddress(cbIndex);
		}

		D3D12_GPU_VIRTUAL_ADDRESS GetCBAddress(FrameIndex frameIndex, int cbIndex = 0)
		{
			if (uploadCB == nullptr)
				Exception::ThrowException(LEVEL_Error, "not upload");

			return uploadCB->GetIndexBufferAddress(GetFrameIndexCB(cbIndex, frameIndex.GetData()));
		}
	private:
		/*
			Const buffer layout:
				cbIndex[0] : frameIndex[0] frameIndex[1] ... frameIndex[frameNum-1]
				cbIndex[1]
				...
				cbIndex[cbNum-1]
		*/
		int GetFrameIndexCB(int cbIndex = 0, int frameIndex = 0)
		{
			return cbIndex * frameNum + frameIndex;
		}

		// const buffer number and frame resource number
		int cbNum;
		int frameNum;

		// CPU const data
		bool haveData;
		std::vector<dataClass> data;

		// GPU const buffer resource
		bool allowChange;
		std::shared_ptr<UploadBuffer<cbClass>> uploadCB = nullptr;

		// Init constbuffer
		bool isInit = false;
		std::vector<InitItem<cbClass>> initItems;
	};
}
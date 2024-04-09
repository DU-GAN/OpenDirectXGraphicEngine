#pragma once

#include "D3DApp.h"
#include "FunctionS.h"
#include "SceneManager.h"
#include "RenderResouce.h"
#include "FrameRsource.h"
#include "GeometricConstructor.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx12.h>

namespace Rendering
{
	class GraphicEngine : public D3DCore
	{
	public:
		GraphicEngine(int _maxModelNum = 0)
		{
			memoryManager = MemoryManage(_maxModelNum);
		}

		GraphicEngine(const GraphicEngine& ge) = delete;
		GraphicEngine& operator=(const GraphicEngine& ge) = delete;

		~GraphicEngine()
		{
			if (md3dDevice != nullptr)
				FlushCommandQueue();
		}

		ID3D12GraphicsCommandList* GetCommandList();
		void CloseCommandList();

		Camera& GetCamera();

		void SetShowGUI(bool show);

		virtual void Draw();

		virtual void LoadModel(UString const& modelPath);

		virtual void SetScreenSize(int screenWidth, int screenHeight);

		virtual void SetFrameResourceNumber(int num);

		virtual void UpdateEngine();

		virtual void SetFrameResourceNum(int _frameResourceNum);

		virtual int GetFrameResourceNum();

	protected:
		void InitIMGUI();

		virtual void ShowGUI(FrameIndex frameIndex);

		virtual void InitRenderPipline();

		virtual void DrawItems(FrameIndex frameIndex, bool enableMsaa = false);

		virtual void UploadPassCB(FrameIndex frameIndex);

		virtual void InitVariable();

		virtual bool Initialize();

		virtual void CreateRtvAndDsvDescriptorHeaps();

		virtual void LoadDefaultShader();

		virtual void LoadDefaultRenderPipline();

		virtual void LoadCoordinateModel(int xLength, int zLength, float dx = 1, float dz = 1);

	protected:
		// other graphics need object
		Camera camera;												
		LightManager lightManager;			

		// base model of scene
		CoordinateColor coordinateColor;							

		// base model and pass constbuffers
		ConstBufferManager<PassCB, PassCB> passCB;
		ConstBufferManager<CoordinateColor, CoordinateColor> coordinateColorCB;

		// memory and renderReource manager
		MemoryManage memoryManager;
		RenderResourceManager renderResourceManager;

		// frame command manager, suport frame resource rendering
		int frameResourceNum = 0;
		FrameCommandManager frameResourceManager;

		// UI and engine need proce event
		std::vector<GEvent> events;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mUiSrvHeap;	

		// Frame command manager
		int frameNum = 1;
		FrameCommandManager frameCommandManager;

		// backgroud buffer clear color
		float backColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float cpuTime, gpuTime;

		bool showLight = false;
		bool showGUI = false;
	};
}
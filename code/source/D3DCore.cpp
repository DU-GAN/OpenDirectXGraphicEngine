#include "D3DCore.h"

namespace Rendering
{
	ID3D12GraphicsCommandList* GraphicEngine::GetCommandList()
	{
		Exception::CheckResult(mCommandList->Reset(mCommandAllocator.Get(), nullptr));
		return mCommandList.Get();
	}

	void GraphicEngine::CloseCommandList()
	{
		Exception::CheckResult(mCommandList->Close());
		ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

		FlushCommandQueue();
	}

	void GraphicEngine::SetScreenSize(int screenWidth, int screenHeight)
	{
		D3DCore::SetScreenSize(screenWidth, screenHeight);
		camera.SetLens(camera.GetFovY(), AspectRatio(), 0.001f, 1000.0f);
	}

	void GraphicEngine::InitRenderPipline()
	{
		LoadDefaultShader();
		LoadDefaultRenderPipline();
	}

	void GraphicEngine::UploadPassCB(FrameIndex frameIndex)
	{
		mTimer.Tick();

		float nearZ;
		float farZ;
		float totalTime;
		float deltaTime;
		Light lights[NUM_MAX_LIGHT];
		PassCB& passData = passCB.GetData();
		
		XMMATRIX view = camera.GetView();
		XMMATRIX proj = camera.GetProj();

		XMMATRIX viewProj = DirectX::XMMatrixMultiply(view, proj);
		XMMATRIX invView = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(view), view);
		XMMATRIX invProj = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(proj), proj);
		XMMATRIX invViewProj = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(viewProj), viewProj);

		XMStoreFloat4x4(&passData.view, DirectX::XMMatrixTranspose(view));
		XMStoreFloat4x4(&passData.invView, DirectX::XMMatrixTranspose(invView));
		XMStoreFloat4x4(&passData.proj, DirectX::XMMatrixTranspose(proj));
		XMStoreFloat4x4(&passData.invProj, DirectX::XMMatrixTranspose(invProj));
		XMStoreFloat4x4(&passData.viewProj, DirectX::XMMatrixTranspose(viewProj));
		XMStoreFloat4x4(&passData.invViewProj, DirectX::XMMatrixTranspose(invViewProj));

		float y = 3.2, r = 2;
		float z = - r * sin(mTimer.TotalTime() / 10);
		float x = - r * cos(mTimer.TotalTime() / 10);

		if (showLight == false)
		{
			renderResourceManager.GetRenderItemWraps()[1].GetWorldTransform().scale = DirectX::XMFLOAT3(0.072, 0.072, 0.072);
			renderResourceManager.GetRenderItemWraps()[1].GetWorldTransform().transln = DirectX::XMFLOAT3(x, y, z);
		}
		renderResourceManager.GetRenderItemWraps()[1].UpdateWorldCB(frameIndex);
		lightManager.GetLights()[0].pos = renderResourceManager.
			GetRenderItemWraps()[1].GetWorldTransform().transln;

		passData.cameraPosW = camera.GetPosition3f();
		passData.lightNum = lightManager.GetNumLight();
		passData.nearZ = camera.GetNearZ();
		passData.farZ = camera.GetFarZ();
		passData.totalTime = mTimer.TotalTime();
		passData.deltaTime = mTimer.DeltaTime();

		for (int i = 0; i < passData.lightNum; ++i)
		{
			passData.lights[i] = lightManager.GetLights()[i];
		}

		passCB.Change(frameIndex, passData);
		coordinateColorCB.Change(frameIndex, coordinateColor);
	}

	Camera& GraphicEngine::GetCamera()
	{
		return camera;
	}

	void GraphicEngine::SetShowGUI(bool show)
	{
		showGUI = show;
	}

	void GraphicEngine::InitVariable()
	{
		{
			lightManager.SetNumLight(1);
			Light light;
			light.pos = XMFLOAT3(4.0f, 2.5f, -4.0f);
			light.radiant = XMFLOAT3(152.0f, 152.0f, 152.0f);
			light.type = 1;
			light.falloffEnd = 1000.0f;
			lightManager.GetLights()[0] = light;
		}

		{
			DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(5.0f, 7.0f, 5.0f);
			DirectX::XMFLOAT3 target = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
			camera.LookAt(pos, target, up);
		}

		{
			coordinateColor.xColor = { 0.9411764705882353f, 0.4862745098039216f, 0.5098039215686275f , 0.72f };
			coordinateColor.zColor = { 0.1607843137254902f, 0.5137254901960784f, 0.7333333333333333f, 0.72f };
			coordinateColor.cColor = { 80.0f / 255,80.0f / 255,80.0f / 255,0.72f };
			backColor[0] = 0.211760f; backColor[1] = 0.160780f; backColor[2] = 0.184310f;
			ChangeMSAAClearValue(backColor[0], backColor[1], backColor[2]);
		}

		{
			passCB = ConstBufferManager<PassCB, PassCB>(1, true, true, frameNum);
			passCB.CreateBufferResource(md3dDevice.Get());
			passCB.GetData().renderType = 0;

			coordinateColorCB = ConstBufferManager<CoordinateColor, CoordinateColor>(1, true, false, frameNum);
			coordinateColorCB.CreateBufferResource(md3dDevice.Get());

			frameCommandManager = FrameCommandManager(frameNum);
			frameCommandManager.Build(md3dDevice.Get());
		}

		{
			renderResourceManager.GetShaders().reserve(20);
			renderResourceManager.GetPiplines().reserve(40);
			renderResourceManager.GetRenderItemWraps().reserve(700);
		}

		{
			memoryManager.GetMemoryG().reserve(100);
			memoryManager.GetMemoryGB().reserve(100);
		}
	}

	void GraphicEngine::CreateRtvAndDsvDescriptorHeaps()
	{
		D3DCore::CreateRtvAndDsvDescriptorHeaps();

		D3D12_DESCRIPTOR_HEAP_DESC SrvHeapDesc;
		SrvHeapDesc.NumDescriptors = 1;
		SrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		SrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		SrvHeapDesc.NodeMask = 0;
		Exception::CheckResult(md3dDevice->CreateDescriptorHeap(
			&SrvHeapDesc, IID_PPV_ARGS(mUiSrvHeap.GetAddressOf())));
	}

	void GraphicEngine::InitIMGUI()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(mhRenderWnd);
		ImGui_ImplDX12_Init(md3dDevice.Get(), SwapChainBufferCount,
			DXGI_FORMAT_R8G8B8A8_UNORM, mUiSrvHeap.Get(),
			mUiSrvHeap.Get()->GetCPUDescriptorHandleForHeapStart(),
			mUiSrvHeap.Get()->GetGPUDescriptorHandleForHeapStart());
	}

	bool GraphicEngine::Initialize() 
	{
		if (!D3DCore::Initialize())                      
			return false;

		InitVariable();

		Exception::CheckResult(mCommandList->Reset(mCommandAllocator.Get(), nullptr));

		InitRenderPipline();

		LoadCoordinateModel(100, 100, 1, 1);

		Exception::CheckResult(mCommandList->Close());
		ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

		FlushCommandQueue();

		memoryManager.GetMemoryG()[0].FreeUploadSpace();

		InitIMGUI();

		return true;
	}

	void GraphicEngine::LoadDefaultShader()
	{
		{
			renderResourceManager.GetShaders().push_back(Shader());
			char vertexPath[] = "./..//resources//shader//Coordinate_Vertex_Shader.hlsl";
			char fragmentPath[] = "./..//resources//shader//Coordinate_Fragment_Shader.hlsl";

			renderResourceManager.GetShaders()[0].SetVertexShader(AnsiToWString(vertexPath), "VS", "vs_5_1");
			renderResourceManager.GetShaders()[0].SetFragmentShader(AnsiToWString(fragmentPath), "PS", "ps_5_1");

			CD3DX12_ROOT_PARAMETER slotRootParameter[3];
			slotRootParameter[0].InitAsConstantBufferView(0);		
			slotRootParameter[1].InitAsConstantBufferView(1);		
			slotRootParameter[2].InitAsConstantBufferView(2);			

			CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(3, slotRootParameter, 0,
				nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

			HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc,
				D3D_ROOT_SIGNATURE_VERSION_1,
				serializedRootSig.GetAddressOf(),
				errorBlob.GetAddressOf());

			Exception::CheckResult(md3dDevice->CreateRootSignature(0,
				serializedRootSig->GetBufferPointer(),
				serializedRootSig->GetBufferSize(),
				IID_PPV_ARGS(&renderResourceManager.GetShaders()[0].GetRootSigature())));
		}

		{
			renderResourceManager.GetShaders().emplace_back();
			char vertexPath[] = "./..//resources//shader//PBR_VertexShader.hlsl";
			char fragmentPath[] = "./..//resources//shader//PBR_FragmentShader.hlsl";

			renderResourceManager.GetShaders()[1].SetVertexShader(AnsiToWString(vertexPath), "VS", "vs_5_1");
			renderResourceManager.GetShaders()[1].SetFragmentShader(AnsiToWString(fragmentPath), "PS", "ps_5_1");

			CD3DX12_ROOT_PARAMETER slotRootParameter[10];

			slotRootParameter[0].InitAsConstantBufferView(0);		 
			slotRootParameter[1].InitAsConstantBufferView(1);		
			slotRootParameter[2].InitAsConstantBufferView(2);		

			CD3DX12_DESCRIPTOR_RANGE texTable[7];
			for (int i = 0; i < 7; ++i)
			{
				texTable[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i);
			}

			slotRootParameter[3].InitAsDescriptorTable(1, &texTable[0], D3D12_SHADER_VISIBILITY_PIXEL);	
			slotRootParameter[4].InitAsDescriptorTable(1, &texTable[1], D3D12_SHADER_VISIBILITY_PIXEL);	
			slotRootParameter[5].InitAsDescriptorTable(1, &texTable[2], D3D12_SHADER_VISIBILITY_PIXEL);	
			slotRootParameter[6].InitAsDescriptorTable(1, &texTable[3], D3D12_SHADER_VISIBILITY_PIXEL);	
			slotRootParameter[7].InitAsDescriptorTable(1, &texTable[4], D3D12_SHADER_VISIBILITY_PIXEL);	
			slotRootParameter[8].InitAsDescriptorTable(1, &texTable[5], D3D12_SHADER_VISIBILITY_PIXEL);	
			slotRootParameter[9].InitAsDescriptorTable(1, &texTable[6], D3D12_SHADER_VISIBILITY_PIXEL);	

			std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplerDesc = GetStaticSamplers();

			CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(10, slotRootParameter, (UINT)samplerDesc.size(), samplerDesc.data(),
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

			HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc,
				D3D_ROOT_SIGNATURE_VERSION_1,
				serializedRootSig.GetAddressOf(),
				errorBlob.GetAddressOf());

			if (errorBlob != nullptr)
			{
				std::cout << (char*)errorBlob->GetBufferPointer() << std::endl;
			}
			Exception::CheckResult(hr);

			Exception::CheckResult(md3dDevice->CreateRootSignature(0,
				serializedRootSig->GetBufferPointer(),
				serializedRootSig->GetBufferSize(),
				IID_PPV_ARGS(&renderResourceManager.GetShaders()[1].GetRootSigature())));
		}

		{
			renderResourceManager.GetShaders().emplace_back();
			char vertexPath[] = "./..//resources//shader//PBR_Animation_VertexShader.hlsl";
			char fragmentPath[] = "./..//resources//shader//PBR_Animation_FragmentShader.hlsl";

			renderResourceManager.GetShaders()[2].SetVertexShader(AnsiToWString(vertexPath), "VS", "vs_5_1");
			renderResourceManager.GetShaders()[2].SetFragmentShader(AnsiToWString(fragmentPath), "PS", "ps_5_1");

			CD3DX12_ROOT_PARAMETER slotRootParameter[11];

			slotRootParameter[0].InitAsConstantBufferView(0);		
			slotRootParameter[1].InitAsConstantBufferView(1);		
			slotRootParameter[2].InitAsConstantBufferView(2);		 
			slotRootParameter[3].InitAsConstantBufferView(3);		

			CD3DX12_DESCRIPTOR_RANGE texTable[7];
			for (int i = 0; i < 7; ++i)
			{
				texTable[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i);
			}

			slotRootParameter[4].InitAsDescriptorTable(1, &texTable[0], D3D12_SHADER_VISIBILITY_PIXEL);
			slotRootParameter[5].InitAsDescriptorTable(1, &texTable[1], D3D12_SHADER_VISIBILITY_PIXEL);	
			slotRootParameter[6].InitAsDescriptorTable(1, &texTable[2], D3D12_SHADER_VISIBILITY_PIXEL);	
			slotRootParameter[7].InitAsDescriptorTable(1, &texTable[3], D3D12_SHADER_VISIBILITY_PIXEL);	
			slotRootParameter[8].InitAsDescriptorTable(1, &texTable[4], D3D12_SHADER_VISIBILITY_PIXEL);
			slotRootParameter[9].InitAsDescriptorTable(1, &texTable[5], D3D12_SHADER_VISIBILITY_PIXEL);	
			slotRootParameter[10].InitAsDescriptorTable(1, &texTable[6], D3D12_SHADER_VISIBILITY_PIXEL);	

			std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplerDesc = GetStaticSamplers();

			CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(11, slotRootParameter, (UINT)samplerDesc.size(), samplerDesc.data(),
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

			HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc,
				D3D_ROOT_SIGNATURE_VERSION_1,
				serializedRootSig.GetAddressOf(),
				errorBlob.GetAddressOf());

			if (errorBlob != nullptr)
			{
				std::cout << (char*)errorBlob->GetBufferPointer() << std::endl;
			}
			Exception::CheckResult(hr);

			Exception::CheckResult(md3dDevice->CreateRootSignature(0,
				serializedRootSig->GetBufferPointer(),
				serializedRootSig->GetBufferSize(),
				IID_PPV_ARGS(&renderResourceManager.GetShaders()[2].GetRootSigature())));
		}

		{
			renderResourceManager.GetShaders().emplace_back();
			char vertexPath[] = "./..//resources//shader//LightVertexShader.hlsl";
			char fragmentPath[] = "./..//resources//shader//LightFragmentShader.hlsl";

			renderResourceManager.GetShaders()[3].SetVertexShader(AnsiToWString(vertexPath), "VS", "vs_5_1");
			renderResourceManager.GetShaders()[3].SetFragmentShader(AnsiToWString(fragmentPath), "PS", "ps_5_1");

			CD3DX12_ROOT_PARAMETER slotRootParameter[2];

			slotRootParameter[0].InitAsConstantBufferView(0);
			slotRootParameter[1].InitAsConstantBufferView(1);

			std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplerDesc = GetStaticSamplers();

			CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter, (UINT)samplerDesc.size(), samplerDesc.data(),
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

			HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc,
				D3D_ROOT_SIGNATURE_VERSION_1,
				serializedRootSig.GetAddressOf(),
				errorBlob.GetAddressOf());

			if (errorBlob != nullptr)
			{
				std::cout << (char*)errorBlob->GetBufferPointer() << std::endl;
			}
			Exception::CheckResult(hr);

			Exception::CheckResult(md3dDevice->CreateRootSignature(0,
				serializedRootSig->GetBufferPointer(),
				serializedRootSig->GetBufferSize(),
				IID_PPV_ARGS(&renderResourceManager.GetShaders()[3].GetRootSigature())));
		}
	}

	void GraphicEngine::LoadDefaultRenderPipline()
	{
		{
			renderResourceManager.GetPiplines().emplace_back();
			renderResourceManager.GetPiplines()[0].SetShader(
				&renderResourceManager.GetShaders()[0]);
			renderResourceManager.GetPiplines()[0].setRenderPiplineName(
				"Render Pipline");
			renderResourceManager.GetPiplines()[0].SetInputDesc(Vertex_G::GetInputLayoutDesc());
			renderResourceManager.GetPiplines()[0].SetPrimiveType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
			renderResourceManager.GetPiplines()[0].SetRTVFormats(&mBackBufferFormat);
			renderResourceManager.GetPiplines()[0].SetDSVFormat(mDepthStencilFormat);

			if (mMSAAMaxSampleCount > 0)
				renderResourceManager.GetPiplines()[0].Build(md3dDevice.Get(), true, mMSAAMaxSampleCount);
			else
				renderResourceManager.GetPiplines()[0].Build(md3dDevice.Get(), false, mMSAAMaxSampleCount);
		}

		{
			renderResourceManager.GetPiplines().emplace_back();
			renderResourceManager.GetPiplines()[1].SetShader(
				&renderResourceManager.GetShaders()[1]);
			renderResourceManager.GetPiplines()[1].setRenderPiplineName(
				"PBR Render Pipline");
			renderResourceManager.GetPiplines()[1].SetInputDesc(Vertex_G::GetInputLayoutDesc());
			renderResourceManager.GetPiplines()[1].SetPrimiveType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
			renderResourceManager.GetPiplines()[1].SetRTVFormats(&mBackBufferFormat);
			renderResourceManager.GetPiplines()[1].SetDSVFormat(mDepthStencilFormat);

			// ����豸֧��MSAA�Ͷ��⹹��MSAA��Ⱦ����
			if (mMSAAMaxSampleCount > 0)
				renderResourceManager.GetPiplines()[1].Build(md3dDevice.Get(), true, mMSAAMaxSampleCount);
			else
				renderResourceManager.GetPiplines()[1].Build(md3dDevice.Get(), false, mMSAAMaxSampleCount);
		}

		{
			renderResourceManager.GetPiplines().emplace_back();
			renderResourceManager.GetPiplines()[2].SetShader(
				&renderResourceManager.GetShaders()[2]);
			renderResourceManager.GetPiplines()[2].setRenderPiplineName(
				"PBR Render Animation Pipline");
			renderResourceManager.GetPiplines()[2].SetInputDesc(Vertex_GB::GetInputLayoutDesc());
			renderResourceManager.GetPiplines()[2].SetPrimiveType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
			renderResourceManager.GetPiplines()[2].SetRTVFormats(&mBackBufferFormat);
			renderResourceManager.GetPiplines()[2].SetDSVFormat(mDepthStencilFormat);

			// ����豸֧��MSAA�Ͷ��⹹��MSAA��Ⱦ����
			if (mMSAAMaxSampleCount > 0)
				renderResourceManager.GetPiplines()[2].Build(md3dDevice.Get(), true, mMSAAMaxSampleCount);
			else
				renderResourceManager.GetPiplines()[2].Build(md3dDevice.Get(), false, mMSAAMaxSampleCount);
		}

		{
			renderResourceManager.GetPiplines().emplace_back();
			renderResourceManager.GetPiplines()[3].SetShader(
				&renderResourceManager.GetShaders()[3]);
			renderResourceManager.GetPiplines()[3].setRenderPiplineName(
				"Light Render Pipline");
			renderResourceManager.GetPiplines()[3].SetInputDesc(Vertex_G::GetInputLayoutDesc());
			renderResourceManager.GetPiplines()[3].SetPrimiveType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
			renderResourceManager.GetPiplines()[3].SetRTVFormats(&mBackBufferFormat);
			renderResourceManager.GetPiplines()[3].SetDSVFormat(mDepthStencilFormat);

			// ����豸֧��MSAA�Ͷ��⹹��MSAA��Ⱦ����
			if (mMSAAMaxSampleCount > 0)
				renderResourceManager.GetPiplines()[3].Build(md3dDevice.Get(), true, mMSAAMaxSampleCount);
			else
				renderResourceManager.GetPiplines()[3].Build(md3dDevice.Get(), false, mMSAAMaxSampleCount);
		}
	}

	void GraphicEngine::DrawItems(FrameIndex frameIndex,bool enableMsaa)
	{
		std::vector<RenderItemWrap>& renderItemWraps = renderResourceManager.GetRenderItemWraps();

		{
			if (renderItemWraps[0].isVisible)
			{
				mCommandList->SetPipelineState(
					renderItemWraps[0].renderPiplineP->GetPiplineState(enableMsaa, mMSAASampleCount).Get());
				mCommandList->SetGraphicsRootSignature(
					renderItemWraps[0].renderPiplineP->GetRootSignature().Get());
				mCommandList->SetGraphicsRootConstantBufferView(1, passCB.GetCBAddress(frameIndex));
				mCommandList->SetGraphicsRootConstantBufferView(2, coordinateColorCB.GetCBAddress(frameIndex));

				renderItemWraps[0].renderItems[0].DrawItem(
					mCommandList.Get(), renderItemWraps[0].primitiveType,
					renderItemWraps[0].worldCB->GetCBAddress(frameIndex));
			}

			if (renderItemWraps[1].isVisible)
			{
				mCommandList->SetPipelineState(
					renderItemWraps[1].renderPiplineP->GetPiplineState(enableMsaa, mMSAASampleCount).Get());
				mCommandList->SetGraphicsRootSignature(
					renderItemWraps[1].renderPiplineP->GetRootSignature().Get());
				mCommandList->SetGraphicsRootConstantBufferView(1, passCB.GetCBAddress(frameIndex));

				renderItemWraps[1].renderItems[0].DrawItem(
					mCommandList.Get(), renderItemWraps[1].primitiveType,
					renderItemWraps[1].worldCB->GetCBAddress(frameIndex));
			}
		}

		{
			if (renderItemWraps.size() < 2)
				return;
			for (int i = 2; i < renderItemWraps.size(); ++i)
			{
				if (renderItemWraps[i].isVisible)
				{
					renderItemWraps[i].UpdateAnimation(frameIndex, mTimer.TotalTime(), false);
					renderItemWraps[i].DrawWrap(
						mCommandList.Get(), frameIndex,
						passCB, mCbvSrvUavDescriptorSize, enableMsaa);
				}
			}
		}
	}

	void GraphicEngine::SetFrameResourceNumber(int num)
	{
		if (frameNum == num)
			return;

		frameNum = num;

		FlushCommandQueue();

		GetCommandList();

		frameCommandManager.Reset(md3dDevice.Get(), frameNum);

		passCB.Reset(md3dDevice.Get(), 1, true, true, frameNum);
		coordinateColorCB.Reset(md3dDevice.Get(), 1, true, false, frameNum);

		std::vector<MemoryWrop<Vertex_G>>& memorys_g
			= memoryManager.GetMemoryG();

		for (int i = 0; i < memorys_g.size(); ++i)
		{
			memorys_g[i].GetGPUMemory().Reset(
				md3dDevice.Get(), mCommandList.Get(),
				mCbvSrvUavDescriptorSize, frameNum);
		}

		std::vector<MemoryWrop<Vertex_GB>>& memorys_gb
			= memoryManager.GetMemoryGB();

		for (int i = 0; i < memorys_gb.size(); ++i)
		{
			memorys_gb[i].GetGPUMemory().Reset(
				md3dDevice.Get(), mCommandList.Get(),
				mCbvSrvUavDescriptorSize, frameNum);
		}

		FreeVector(renderResourceManager.GetRenderItemWraps());

		for (int i = 0; i < memoryManager.GetMemoryG().size(); ++i)
		{
			RenderItemWrap renderItemWrap = memoryManager.GetMemoryG()[i].GetRenderItemWrap();

			renderItemWrap.SetRenderPipline(&renderResourceManager.GetPiplines()[1]);
			renderItemWrap.SetPrimitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			renderResourceManager.GetRenderItemWraps().push_back(renderItemWrap);
		}

		for (int i = 0; i < memoryManager.GetMemoryGB().size(); ++i)
		{
			RenderItemWrap renderItemWrap = memoryManager.GetMemoryGB()[i].GetRenderItemWrap();

			renderItemWrap.SetRenderPipline(&renderResourceManager.GetPiplines()[2]);
			renderItemWrap.SetPrimitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			renderResourceManager.GetRenderItemWraps().push_back(renderItemWrap);
		}
		

		CloseCommandList();
	}

	void GraphicEngine::SetFrameResourceNum(int _frameResourceNum)
	{
		frameResourceNum = _frameResourceNum;
	}

	int GraphicEngine::GetFrameResourceNum()
	{
		return frameResourceNum;
	}

	void GraphicEngine::UpdateEngine()
	{
		FlushCommandQueue();

		D3D12_FILL_MODE mode;
		D3D12_CULL_MODE mode2;
		D3D12_RASTERIZER_DESC desc;
		std::vector<std::wstring> filePaths;

		for (int i = events.size() - 1; i >= 0; --i)
		{
			switch (events[i].type)
			{
			case GEvent::Type::UpdateMSAAState:
				if (events[i].i1 != 0)
				{
					SetMsaaState(true);
					SetMSAASampleCount(events[i].i1 * 4);
				}
				else
				{
					SetMsaaState(false);
				}
				break;
			case GEvent::Type::UpdateFullScreen:
				SetFullScreenState(events[i].b1);
				break;
			case GEvent::Type::UpdateFrameResource:
				SetFrameResourceNumber(events[i].i1);
				break;
			case GEvent::Type::LoadModel:
				filePaths = OpenFileDialogS(mhRenderWnd);
				for (int i = 0; i < filePaths.size(); ++i)
				{
					LoadModel(filePaths[i]);
				}
				break;
			case GEvent::Type::UpdateRenderMode:
				if (events[i].b1)
				{
					mode = (events[i].i1 == 0 ? D3D12_FILL_MODE_SOLID : D3D12_FILL_MODE_WIREFRAME);
					for (int i = 0; i < renderResourceManager.GetPiplines().size(); ++i)
					{
						if (renderResourceManager.GetPiplines()[i].GetPiplineStateDesc()
							.RasterizerState.FillMode != mode)
						{
							desc = renderResourceManager.GetPiplines()[i].
								GetPiplineStateDesc().RasterizerState;
							desc.FillMode = mode;
							renderResourceManager.GetPiplines()[i].SetRasterizerState(desc);
							renderResourceManager.GetPiplines()[i].Reset(md3dDevice.Get(),
								mMSAAMaxSampleCount >= 4, mMSAAMaxSampleCount);
						}
					}
				}else
				{
					passCB.GetData().renderType = events[i].i1;
				}
				break;
			case GEvent::Type::UpdateCullBackFace:
				if (events[i].i1 == 0)
					mode2 = D3D12_CULL_MODE_NONE;
				else
					mode2 = (events[i].i1 == 1 ? D3D12_CULL_MODE_FRONT : D3D12_CULL_MODE_BACK);
				for (int i = 0; i < renderResourceManager.GetPiplines().size(); ++i)
				{
					if (renderResourceManager.GetPiplines()[i].GetPiplineStateDesc()
						.RasterizerState.CullMode != mode2)
					{
						desc = renderResourceManager.GetPiplines()[i].
							GetPiplineStateDesc().RasterizerState;
						desc.CullMode = mode2;
						renderResourceManager.GetPiplines()[i].SetRasterizerState(desc);
						renderResourceManager.GetPiplines()[i].Reset(md3dDevice.Get(),
							mMSAAMaxSampleCount >= 4, mMSAAMaxSampleCount);
					}
				}
				break;
			}
		}
		FreeVector(events);
	}

	void GraphicEngine::ShowGUI(FrameIndex frameIndex)
	{
		if (!showGUI)
			return;

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(200, 150));

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoNav;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoScrollbar;
		
		if (ImGui::Begin("Framse Information", NULL, window_flags))
		{
			ImGui::SetWindowFontScale(1.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, 
				ImVec4(237.0f /255, 85.0f / 255, 106.0f/255, 1.0f));

			ImGui::Text(frameInfo.GetC_Str());
			
			float time = cpuTime + gpuTime;
			float cpu = cpuTime / time * 100;
			cpu = Max(0.0f, cpu);
			float gpu = gpuTime / time * 100;
			gpu = Min(100.0f, gpu);
			std::string cpuStr = std::to_string(cpu);
			std::string gpuStr = std::to_string(gpu);
			cpuStr.resize(Min<int>(4, cpuStr.size()));
			gpuStr.resize(Min<int>(4, gpuStr.size()));
			std::string s = "CPU:" + cpuStr + "  GPU:" + gpuStr;

			ImGui::Text(s.c_str());

			std::string ro = std::string("Resolution: ") +
				std::to_string(mClientWidth) + "x" + std::to_string(mClientHeight);
			ImGui::Text(ro.c_str());

			int i = GetDpiForWindow(mhRenderWnd);
			std::string dpi = std::string("DPI: ") + std::to_string(i);
			ImGui::Text(dpi.c_str());
			ImGui::PopStyleColor();
		}
		ImGui::End();

		window_flags = 0;
		if (ImGui::Begin("Rendering option", NULL, window_flags))
		{
			ImGui::SetWindowFontScale(1.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, 
				ImVec4(237.0f / 255, 85.0f / 255, 106.0f / 255, 1.0f));
			if (ImGui::Checkbox(" Sync", &enableSync))
			{

			}
			ImGui::SameLine();
			static bool fullScreen = false;
			if (ImGui::Checkbox(" Full Screen", &fullScreen))
			{
				events.emplace_back(GEvent::Type::UpdateFullScreen);
				events.back().b1 = fullScreen;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox(" Show Light", &showLight))
			{
				
			}
			renderResourceManager.GetRenderItemWraps()[1].isVisible = showLight;
			{
				enum Element { DISABLE, E4X, E8X, Element_COUNT };
				std::vector<UString> msaaSampleCounts;
				static int elem1 = DISABLE;
				const char* elems_names[Element_COUNT] = {"DISABLE", "4X", "8X",};
				const char* elem_name = (elem1 >= 0 && elem1 < Element_COUNT) ? elems_names[elem1] :
					"Unknown";

				int index = 0;
				switch (mMSAAMaxSampleCount)
				{
				case 0:
					index = 0;
					break;
				case 4:
					index = 1;
					break;
				default:
					index = 2;
					break;
				}
				if (ImGui::SliderInt(" MSAA", &elem1, 0, index, elem_name))
				{
					GEvent& et = AddElementVector(events);
					et.type = GEvent::Type::UpdateMSAAState;
					et.i1 = elem1;
				}
			}
			{
				enum Element { Fill, Wireframe, Element_COUNT };
				static int elem2 = Fill;
				const char* elems_names[Element_COUNT] = { "Fill","Wireframe"};
				const char* elem_name = (elem2 >= 0 && elem2 < Element_COUNT) ? elems_names[elem2] :
					"Unknown";

				if (ImGui::SliderInt(" Fill Mode", &elem2, 0, 1, elem_name))
				{
					GEvent& et = AddElementVector(events);
					et.b1 = true;
					et.type = GEvent::Type::UpdateRenderMode;
					et.i1 = elem2;
				}
			}
			{
				enum Element { Render, Material, Entity1, Entity2, Element_COUNT };
				static int elem3 = Render;
				const char* elems_names[Element_COUNT] = { "Render","Material","Entity1","Entity2" };
				const char* elem_name = (elem3 >= 0 && elem3 < Element_COUNT) ? elems_names[elem3] :
					"Unknown";

				if (ImGui::SliderInt(" Render Mode", &elem3, 0, 3, elem_name))
				{
					GEvent& et = AddElementVector(events);
					et.b1 = false;
					et.type = GEvent::Type::UpdateRenderMode;
					et.i1 = elem3;
				}
			}
			{
				enum Element { Close, FrontFace, BackFace, Element_COUNT };
				static int elem4 = Close;
				const char* elems_names[Element_COUNT] = { "Close","Front Face","Back Face"};
				const char* elem_name = (elem4 >= 0 && elem4 < Element_COUNT) ? elems_names[elem4] :
					"Unknown";

				if (ImGui::SliderInt(" Face Culling", &elem4, 0, 2, elem_name))
				{
					GEvent& et = AddElementVector(events);
					et.b1 = false;
					et.type = GEvent::Type::UpdateCullBackFace;
					et.i1 = elem4;
				}
			}
			/*
						{
				static int i1 = 1;
				ImGui::SliderInt(" Frame resource number", &i1, 1, 24);
				GEvent& et = AddElementVector(events);
				et.type = GEvent::Type::UpdateFrameResource;
				et.i1 = i1;
			}
			*/

			ImGui::PopStyleColor();

		}
		ImGui::End();

		window_flags = 0;
		if (ImGui::Begin("Function option", NULL, window_flags))
		{
			if (ImGui::Button(" Load Model File"))
			{
				events.emplace_back(GEvent::Type::LoadModel);
			}
		}
		ImGui::End();

		window_flags = 0;
		window_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
		if (ImGui::Begin("Model Manager", NULL, window_flags))
		{
			static std::vector<ModelInfo> modelInfos_G;
			modelInfos_G.resize(memoryManager.GetMemoryG().size());

			static std::vector<ModelInfo> modelInfos_GB;
			modelInfos_GB.resize(memoryManager.GetMemoryGB().size());

			int gCount = -1, gbCount = -1;

			for (int i = 0; i < renderResourceManager.GetRenderItemWraps().size(); ++i)
			{
				RenderItemWrap& renderItemWrap = renderResourceManager.GetRenderItemWraps()[i];
				GPUMemory* gpuMemory = renderItemWrap.memory;

				if (!renderItemWrap.suportAnimation)
				{
					gCount += 1;
					std::string str = std::string("[") + std::to_string(i) + "] : " +
						memoryManager.GetMemoryG()[gCount].GetModel().GetName().GetString();
					std::string s;

					if (ImGui::CollapsingHeader(str.c_str())) {
						s = "Visible##G" + std::to_string(gCount);
						if (ImGui::Checkbox(s.c_str(), &renderItemWrap.isVisible))
						{
						}
						ImGui::SameLine();
						s = "Reset##G" + std::to_string(gCount);
						if (ImGui::Button(s.c_str()))
						{
							modelInfos_G[gCount] = ModelInfo();
						}

						s = ("Position##G" + std::to_string(gCount));
						ImGui::SliderFloat3(s.c_str(), modelInfos_G[gCount].position, -100, 100);
						s = "Rotation##G" + std::to_string(gCount);
						ImGui::SliderFloat3(s.c_str(), modelInfos_G[gCount].rotate, -100, 100);
						s = "Scaleing##G" + std::to_string(gCount);
						ImGui::SliderFloat(s.c_str(), modelInfos_G[gCount].scale, 0, 1);

						WorldTransform& wf = renderItemWrap.GetWorldTransform();

						wf.SetScale(modelInfos_G[gCount].scale[0], modelInfos_G[gCount].scale[0], modelInfos_G[gCount].scale[0]);
						wf.SetRotate(modelInfos_G[gCount].rotate[0], modelInfos_G[gCount].rotate[1], modelInfos_G[gCount].rotate[2]);
						wf.SetTranslate(modelInfos_G[gCount].position[0], modelInfos_G[gCount].position[1], modelInfos_G[gCount].position[2]);

						renderItemWrap.UpdateWorldCB(frameIndex);
					}
				}
				else
				{
					gbCount += 1;
					std::string str = std::string("[") + std::to_string(i) + "] : " +
						memoryManager.GetMemoryGB()[gbCount].GetModel().GetName().GetString();
					std::string s;

					if (ImGui::CollapsingHeader(str.c_str())) {
						s = "Visible##" + std::to_string(gbCount);
						if (ImGui::Checkbox(s.c_str(), &renderItemWrap.isVisible))
						{
						}
						ImGui::SameLine();

						s = "Reset##" + std::to_string(gbCount);
						if (ImGui::Button(s.c_str()))
						{
							modelInfos_GB[gbCount] = ModelInfo();
						}
						ImGui::SameLine();

						s = "LoadAnimationFile##" + std::to_string(gbCount);
						if (ImGui::Button(s.c_str()))
						{
							std::vector<std::wstring> files = OpenFileDialogS(mhRenderWnd);
							for (int j = 0; j < files.size(); ++j)
							{
								Assimp::Importer importer;
								Model<Vertex_GB>::LoadAnimations(
									Model<Vertex_GB>::LoadModelScene(files[j], importer),
									memoryManager.GetMemoryGB()[gbCount].GetModel());
							}
						}

						s = "Position##" + std::to_string(gbCount);
						ImGui::SliderFloat3(s.c_str(), modelInfos_GB[gbCount].position, -100, 100);
						s = "Rotation##" + std::to_string(gbCount);
						ImGui::SliderFloat3(s.c_str(), modelInfos_GB[gbCount].rotate, -10, 10);
						s = "Scaleing##" + std::to_string(gbCount);
						ImGui::SliderFloat(s.c_str(), modelInfos_GB[gbCount].scale, 0, 1);

						WorldTransform& wf = renderItemWrap.GetWorldTransform();

						wf.SetScale(modelInfos_GB[gbCount].scale[0], 
							modelInfos_GB[gbCount].scale[0], modelInfos_GB[gbCount].scale[0]);
						wf.SetRotate(modelInfos_GB[gbCount].rotate[0], 
							modelInfos_GB[gbCount].rotate[1], modelInfos_GB[gbCount].rotate[2]);
						wf.SetTranslate(modelInfos_GB[gbCount].position[0], 
							modelInfos_GB[gbCount].position[1], modelInfos_GB[gbCount].position[2]);

						renderItemWrap.UpdateWorldCB(frameIndex);

						std::vector<Animation>* animationsP =
							renderItemWrap.GetAnimationsP();
						modelInfos_GB[gbCount].animationName.resize(2 + animationsP->size());

						modelInfos_GB[gbCount].animationName[0] = "static";
						for (int j = 0; j < animationsP->size(); ++j)
						{
							modelInfos_GB[gbCount].animationName[j + 1] = (*animationsP)[j].name.GetString();
						}

						const char* const* strs = ConvertVectorToStringArray(modelInfos_GB[gbCount].animationName);

						bool isChange = false;
						s = "Animation list##" + std::to_string(gbCount);
						if (ImGui::ListBox(s.c_str(), &modelInfos_GB[gbCount].animationIndex,
							strs, modelInfos_GB[gbCount].animationName.size() - 1, modelInfos_GB[gbCount].animationName.size()))
						{
							isChange = true;
							if (modelInfos_GB[gbCount].animationIndex == 0)
								renderItemWrap.StopPlayAnimaiton(frameIndex);
							else
							{
								renderItemWrap.PlayAnimation(&(*animationsP)[modelInfos_GB[gbCount].animationIndex - 1]);
							}
						}
						if (renderItemWrap.animator.m_CurrentAnimation != nullptr)
						{
							s = "timeSpeed##" + std::to_string(gbCount);
							ImGui::SliderFloat(s.c_str(), &modelInfos_GB[gbCount].animationSpeed, 0, 10);
							renderItemWrap.animator.timeSpace = modelInfos_GB[gbCount].animationSpeed;
							if (modelInfos_GB[gbCount].timeRange[0] < 0 || modelInfos_GB[gbCount].timeRange[1] < 0 || isChange)
							{
								modelInfos_GB[gbCount].timeRange[0] = 0;
								modelInfos_GB[gbCount].timeRange[1] =
									renderItemWrap.animator.m_CurrentAnimation->m_duration;
								isChange = false;
							}
							s = "Animation Range##" + std::to_string(gbCount);
							ImGui::DragFloatRange2(s.c_str(),
								&modelInfos_GB[gbCount].timeRange[0],
								&modelInfos_GB[gbCount].timeRange[1],
								1.0f, 0.0f, renderItemWrap.animator.m_CurrentAnimation->m_duration);
							renderItemWrap.animator.SetDebugMode(
								true, modelInfos_GB[gbCount].timeRange[0], modelInfos_GB[gbCount].timeRange[1]);

							float k = renderItemWrap.animator.m_CurrentTime /
								renderItemWrap.animator.m_CurrentAnimation->m_duration;
							ImGui::ProgressBar(k, ImVec2(0.0f, 0.0f), (std::to_string(100 * k) + "%").c_str());
							ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

							s = "Animation Process##" + std::to_string(gbCount);
							ImGui::Text(s.c_str());
						}
						delete strs;
					}
				}

			}
		}
		ImGui::End();

		//bool show_demo_window = true;
		//ImGui::ShowDemoWindow(&show_demo_window);

		mCommandList->SetDescriptorHeaps(1, mUiSrvHeap.GetAddressOf());
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList.Get());
	}

	void GraphicEngine::Draw()
	{
		if (md3dDevice.Get()->GetDeviceRemovedReason() != S_OK)
		{
			HRESULT res = md3dDevice.Get()->GetDeviceRemovedReason();
			Exception::ThrowException(LEVEL_Warning, "Remove device");
		}

		CalculateFrameStats();
		
		static float time = 0;
		float dTime = mTimer.NowTotalTime() - time;
		time = mTimer.NowTotalTime();

		std::vector<RenderItemWrap>& renderItemWraps = renderResourceManager.GetRenderItemWraps();
		if (renderItemWraps.size() < 1)
			return;

		ID3D12CommandAllocator* frameCommandAllocator = nullptr;
		FrameIndex frameIndex = frameCommandManager.BeginNewFrame(mCommandQueue.Get(), mFence.Get(), frameCommandAllocator);

		UploadPassCB(frameIndex);

		Exception::CheckResult(frameCommandAllocator->Reset());
		mCommandList->Reset(frameCommandAllocator, nullptr);

		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		if (mMsaaState)
		{
			mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
				mMsaaRenderTarget.Get(), D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
				D3D12_RESOURCE_STATE_RENDER_TARGET));

			auto rtvDescriptor = mMsaaRtvHeap->GetCPUDescriptorHandleForHeapStart();
			auto dsvDescriptor = mMsaaDsvHeap->GetCPUDescriptorHandleForHeapStart();

			mCommandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, &dsvDescriptor);
			mCommandList->ClearRenderTargetView(rtvDescriptor, backColor, 0, nullptr);
			mCommandList->ClearDepthStencilView(dsvDescriptor, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

			DrawItems(frameIndex, true);

			D3D12_RESOURCE_BARRIER barriers[2] =
			{
				CD3DX12_RESOURCE_BARRIER::Transition(
					mMsaaRenderTarget.Get(),
					D3D12_RESOURCE_STATE_RENDER_TARGET,
					D3D12_RESOURCE_STATE_RESOLVE_SOURCE),
				CD3DX12_RESOURCE_BARRIER::Transition(
					CurrentBackBuffer(),
					D3D12_RESOURCE_STATE_PRESENT,
					D3D12_RESOURCE_STATE_RESOLVE_DEST)
			};

			mCommandList->ResourceBarrier(2, barriers);

			mCommandList->ResolveSubresource(CurrentBackBuffer(),
				0, mMsaaRenderTarget.Get(), 0, mBackBufferFormat);

			D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				CurrentBackBuffer(),
				D3D12_RESOURCE_STATE_RESOLVE_DEST,
				D3D12_RESOURCE_STATE_PRESENT);

			mCommandList->ResourceBarrier(1, &barrier);

			mCommandList->ResourceBarrier(1,
				&CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
					D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

			mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

			ShowGUI(frameIndex);

			mCommandList->ResourceBarrier(1,
				&CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
					D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		}
		else
		{
			mCommandList->ResourceBarrier(1,
				&CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
					D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

			mCommandList->ClearRenderTargetView(CurrentBackBufferView(), backColor, 0, nullptr);

			mCommandList->ClearDepthStencilView(DepthStencilView(),
				D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
				1.0f, 0, 0, nullptr);

			mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

			DrawItems(frameIndex, false);

			ShowGUI(frameIndex);

			mCommandList->ResourceBarrier(1,
				&CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
					D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));	
		}

		Exception::CheckResult(mCommandList->Close());

		ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		PresentSwapChain();

		frameCommandManager.EndFrame(mCommandQueue.Get(), mFence.Get(), mCurrentFence);

		static float c = 0;
		c += dTime;
		if (c >= 1.0f)
		{
			gpuTime = mTimer.NowTotalTime() - time;
			cpuTime = dTime - gpuTime;		
			c = 0.0f;
		}
	}

	void GraphicEngine::LoadCoordinateModel(int xLength, int zLength, float dx, float dz)
	{
		memoryManager.GetMemoryG().emplace_back();

		MemoryWrop<Vertex_G>& memoryWrop = memoryManager.GetMemoryG().back();

		Model<Vertex_G>& model = memoryWrop.GetModel();

		int count = 2 * xLength / dx + 2 * zLength / dz + 100;
		Mesh<Vertex_G> mesh(count, count);
		Vertex_G vertex;
		std::uint16_t index = 0;

		for (float x = -xLength; x <= xLength; x += dx)
		{
			vertex.position = DirectX::XMFLOAT3(x, 0, zLength);
			mesh.GetVertexs().push_back(vertex);
			mesh.AddIndex(index);
			index += 1;

			vertex.position = DirectX::XMFLOAT3(x, 0, -zLength);
			mesh.GetVertexs().push_back(vertex);
			mesh.AddIndex(index);
			index += 1;
		}
		for (float z = -zLength; z <= zLength; z += dz)
		{
			vertex.position = DirectX::XMFLOAT3(xLength, 0, z);
			mesh.GetVertexs().push_back(vertex);
			mesh.AddIndex(index);
			index += 1;

			vertex.position = DirectX::XMFLOAT3(-xLength, 0, z);
			mesh.GetVertexs().push_back(vertex);
			mesh.AddIndex(index);
			index += 1;
		}

		mesh.SetName("Coordinate_Mesh");
		model.SetName("Coordinate_Mode");
		model.GetMaterialManager().AddDefaultMaterial();

		mesh.SetPBRMaterilaIndex(model.GetMaterialManager().GetDefaultMaterialIndex());
		model.GetMeshs().push_back(std::move(mesh));

		RenderItemWrap renderItemWrap = 
			memoryWrop.LoadModelToGPU(md3dDevice.Get(), mCommandList.Get(), mCbvSrvUavDescriptorSize);

		renderItemWrap.SetRenderPipline(&renderResourceManager.GetPiplines()[0]);
		renderItemWrap.SetPrimitiveType(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		renderResourceManager.GetRenderItemWraps().push_back(renderItemWrap);

		{
			memoryManager.GetMemoryG().emplace_back();

			MemoryWrop<Vertex_G>& memoryWrop = memoryManager.GetMemoryG().back();
			GeometricConstructor::LoadCubeGeometric(memoryWrop.GetModel());
			memoryWrop.GetModel().SetName("Light Cube");
			memoryWrop.GetModel().GetMaterialManager().AddDefaultMaterial();
			memoryWrop.GetModel().GetMeshs()[0].SetPBRMaterilaIndex(model.
				GetMaterialManager().GetDefaultMaterialIndex());
			
			RenderItemWrap renderItemWrap =
				memoryWrop.LoadModelToGPU(md3dDevice.Get(), mCommandList.Get(), mCbvSrvUavDescriptorSize);

			renderItemWrap.SetRenderPipline(&renderResourceManager.GetPiplines()[3]);
			renderItemWrap.SetPrimitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			renderItemWrap.isVisible = false;

			renderResourceManager.GetRenderItemWraps().push_back(renderItemWrap);
		}
	}

	void GraphicEngine::LoadModel(UString const& modelPath)
	{
		GetCommandList();

		Assimp::Importer importer;

		const aiScene* scene = Model<Vertex_G>::LoadModelScene(modelPath, importer);

		int modelType = Model<Vertex_G>::FindModelType(scene);

		if (modelType == 0)
		{
			memoryManager.GetMemoryG().emplace_back();
			MemoryWrop<Vertex_G>& memoryWrop = memoryManager.GetMemoryG().back();

			memoryWrop.LoadModelToCPU(modelPath, scene, modelPath);
			RenderItemWrap renderItemWrap = memoryWrop.LoadModelToGPU(
				md3dDevice.Get(), mCommandList.Get(), mCbvSrvUavDescriptorSize);

			renderItemWrap.SetRenderPipline(&renderResourceManager.GetPiplines()[1]);
			renderItemWrap.SetPrimitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			renderResourceManager.GetRenderItemWraps().push_back(renderItemWrap);

			CloseCommandList();
			FlushCommandQueue();
			memoryWrop.FreeUploadSpace();
		}
		else if (modelType == 1)
		{
			memoryManager.GetMemoryGB().emplace_back();
			MemoryWrop<Vertex_GB>& memoryWrop = memoryManager.GetMemoryGB().back();

			memoryWrop.LoadModelToCPU(modelPath, scene, modelPath);
			RenderItemWrap renderItemWrap = memoryWrop.LoadModelToGPU(
				md3dDevice.Get(), mCommandList.Get(), mCbvSrvUavDescriptorSize);

			renderItemWrap.SetRenderPipline(&renderResourceManager.GetPiplines()[2]);
			renderItemWrap.SetPrimitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			renderResourceManager.GetRenderItemWraps().push_back(renderItemWrap);

			CloseCommandList();
			FlushCommandQueue();
			memoryWrop.FreeUploadSpace();
		}
	}
}
#include "D3DApp.h"

namespace Rendering
{
    void D3DCore::Build(HINSTANCE _hInstance, HWND renderHWindow)
    {
        mAppBuild = true;

        mhAppInst = _hInstance;
        mhRenderWnd = renderHWindow;

        RECT rect;
        GetWindowRect(renderHWindow, &rect);

        mClientWidth = rect.right - rect.left;
        mClientHeight = rect.bottom - rect.top;

        Initialize();

        mTimer.Reset();
    }

    HINSTANCE D3DCore::AppInst()
    {
        return mhAppInst;
    }

    HWND      D3DCore::RenderWnd()
    {
        return mhRenderWnd;
    }

    float     D3DCore::AspectRatio()
    {
        return static_cast<float>(mClientWidth) / mClientHeight;
    }

    bool D3DCore::GetMsaaState()
    {
        return mMsaaState;
    }

    int D3DCore::GetMsaaSampleCount()const
    {
        return mMSAASampleCount;
    }

    int D3DCore::GetMsaaMaxSampleCount()const
    {
        return mMSAAMaxSampleCount;
    }

    void D3DCore::SetMsaaState(bool enable)
    {
        if (mMSAAMaxSampleCount == 0)
        {
            Exception::ThrowException(LEVEL_Prompt, "Device no suport msaa");
            return;
        }
        if (mMsaaState != enable)
        {
            mMsaaState = enable;
        }
    }

    void D3DCore::ChangeMSAAClearValue(float clearColorR, float clearColorG, float clearColorB)
    {
        if (mMSAAMaxSampleCount >= 4)
        {
            FlushCommandQueue();

            CreateMSAAResource(clearColorR, clearColorG, clearColorB);
        }
        else
        {
            Exception::ThrowException(LEVEL_Prompt, "Device no suport msaa");
        }
    }

    void D3DCore::SetMSAASampleCount(int sampleCount)
    {
        if (sampleCount <= mMSAAMaxSampleCount && sampleCount >= 4
            && sampleCount != mMSAASampleCount)
        {
            mMSAASampleCount = sampleCount;

            FlushCommandQueue();

            CreateMSAAResource();
        }
    }

    bool D3DCore::Initialize()                     
    {
        if (!InitDirect3D())
        {
            Exception::ThrowException(LEVEL_Error, "D3D init faild");
            return false;
        }

        OnResize();                              

        return true;
    }

    void D3DCore::Draw()
    {
        
    }

    void D3DCore::CreateRtvAndDsvDescriptorHeaps()
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
        rtvHeapDesc.NumDescriptors = SwapChainBufferCount;  
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;  
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtvHeapDesc.NodeMask = 0;
        Exception::CheckResult(md3dDevice->CreateDescriptorHeap(     
            &rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())
        ));

        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;             
        dsvHeapDesc.NumDescriptors = 1;                     
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; 
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        dsvHeapDesc.NodeMask = 0;
        Exception::CheckResult(md3dDevice->CreateDescriptorHeap(
            &dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
    }

    void D3DCore::OnResize()
    {
        if (mClientHeight <= 0 || mClientWidth <= 0)
        {
            Exception::ThrowException(LEVEL_Prompt, "height or width is faild number");
            return;
        }

        if (!md3dDevice || !mSwapChain || !mCommandAllocator)
        {
            Exception::ThrowException(LEVEL_Error, "d3d obect is null");
        }

        FlushCommandQueue();

        if (mMSAAMaxSampleCount != 0)
            CreateMSAAResource(backColor[0], backColor[1], backColor[2]);

        Exception::CheckResult(mCommandList->Reset(mCommandAllocator.Get(), nullptr));

        for (int i = 0; i < SwapChainBufferCount; ++i)
            mSwapChainBuffer[i].Reset();
        mDepthStencilBuffer.Reset();

        Exception::CheckResult(mSwapChain->ResizeBuffers(
            SwapChainBufferCount,           
            mClientWidth, mClientHeight,   
            mBackBufferFormat,
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING));

        mCurrBackBuffer = 0;

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());

        for (UINT i = 0; i < SwapChainBufferCount; ++i)
        {
            Exception::CheckResult(
                mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));

            md3dDevice->CreateRenderTargetView(
                mSwapChainBuffer[i].Get(),  
                nullptr,                   
                rtvHeapHandle);             

            rtvHeapHandle.Offset(1, mRtvDescriptorSize);
        }

        D3D12_RESOURCE_DESC depthStencilDesc;

        depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; 
        depthStencilDesc.Alignment = 0;
        depthStencilDesc.Width = mClientWidth;                           
        depthStencilDesc.Height = mClientHeight;                         
        depthStencilDesc.DepthOrArraySize = 1;                           
        depthStencilDesc.MipLevels = 1;                                  
        depthStencilDesc.Format = mDepthStencilFormat;                   
        depthStencilDesc.SampleDesc.Count = 1;   
        depthStencilDesc.SampleDesc.Quality = 0; 
        depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;          
        depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE optClear;                             
        optClear.Format = mDepthStencilFormat;                  
        optClear.DepthStencil.Depth = 1.0f;                     
        optClear.DepthStencil.Stencil = 0;                      

        CD3DX12_HEAP_PROPERTIES d3dxObject1(D3D12_HEAP_TYPE_DEFAULT);

        Exception::CheckResult(md3dDevice->CreateCommittedResource(      
            &d3dxObject1,                                       
            D3D12_HEAP_FLAG_NONE,                               
            &depthStencilDesc,                                  
            D3D12_RESOURCE_STATE_COMMON,                        
            &optClear,                                          
            IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));

        md3dDevice->CreateDepthStencilView(
            mDepthStencilBuffer.Get(),
            nullptr,
            DepthStencilView()
        );

        CD3DX12_RESOURCE_BARRIER cd3dxObject2 = CD3DX12_RESOURCE_BARRIER::Transition( 
            mDepthStencilBuffer.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_DEPTH_WRITE);

        mCommandList->ResourceBarrier(            
            1,
            &cd3dxObject2);

        Exception::CheckResult(mCommandList->Close());   
        ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
        mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);  

        FlushCommandQueue();

        mScreenViewport.TopLeftX = 0;  
        mScreenViewport.TopLeftY = 0;
        mScreenViewport.Width = static_cast<float>(mClientWidth);   
        mScreenViewport.Height = static_cast<float>(mClientHeight);
        mScreenViewport.MinDepth = 0.0f;    
        mScreenViewport.MaxDepth = 1.0f;

        mScissorRect = { 0, 0, mClientWidth, mClientHeight };

        return;
    }

    bool D3DCore::InitDirect3D()
    {
        mClientHeight = max(mClientHeight, 100);
        mClientWidth = max(mClientWidth, 100);

#if defined(DEBUG) || defined(_DEBUG) 
        {
            ComPtr<ID3D12Debug> debugController;
            Exception::CheckResult(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
            debugController->EnableDebugLayer();
        }
#endif

        Exception::CheckResult(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));

        HRESULT hardwareResult = D3D12CreateDevice(
            nullptr,
            D3D_FEATURE_LEVEL_12_0,
            IID_PPV_ARGS(&md3dDevice));

        if (FAILED(hardwareResult))
        {
            ComPtr<IDXGIAdapter> pWarpAdapter;
            Exception::CheckResult(
                mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

            Exception::CheckResult(D3D12CreateDevice(
                pWarpAdapter.Get(),
                D3D_FEATURE_LEVEL_12_0,
                IID_PPV_ARGS(&md3dDevice)
            ));
        }

        Exception::CheckResult(md3dDevice->CreateFence(
            0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));

        mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
        msQualityLevels.Format = mBackBufferFormat;                         
        msQualityLevels.SampleCount = 4;                                    
        msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE; 
        msQualityLevels.NumQualityLevels = 0;                               

        for (int i = 4; i <= 64; i *= 2)
        {
            msQualityLevels.SampleCount = i;

            Exception::CheckResult(md3dDevice->CheckFeatureSupport(
                D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
                &msQualityLevels,
                sizeof(msQualityLevels)));

            if (msQualityLevels.NumQualityLevels > 0)
            {
                mMSAAMaxSampleCount = i;
            }
            else
                break;
        }

        mMSAASampleCount = (mMSAAMaxSampleCount != 0) ? 4 : 0;

        LogAdapters();

        CreateCommandObjects();

        CreateSwapChain();

        CreateRtvAndDsvDescriptorHeaps();

        SetResolutionIndex(resolutionRatios.size() - 1);

        return true;
    }

    void D3DCore::CreateCommandObjects()
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};

        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        Exception::CheckResult(md3dDevice->CreateCommandQueue(
            &queueDesc, IID_PPV_ARGS(&mCommandQueue)
        ));

        Exception::CheckResult(md3dDevice->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,                 
            IID_PPV_ARGS(mCommandAllocator.GetAddressOf())
        ));

        Exception::CheckResult(md3dDevice->CreateCommandList(
            0,                                       
            D3D12_COMMAND_LIST_TYPE_DIRECT,          
            mCommandAllocator.Get(),                 
            nullptr,                                 
            IID_PPV_ARGS(mCommandList.GetAddressOf())));

        mCommandList->Close();
    }

    void D3DCore::CreateSwapChain()
    {
        mSwapChain.Reset();

        DXGI_SWAP_CHAIN_DESC sd;

        sd.BufferDesc.Width = mClientWidth;             
        sd.BufferDesc.Height = mClientHeight;           
        sd.BufferDesc.RefreshRate.Numerator = 60;       
        sd.BufferDesc.RefreshRate.Denominator = 1;      
        sd.BufferDesc.Format = mBackBufferFormat;       
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
        sd.SampleDesc.Count = 1;                         
        sd.SampleDesc.Quality = 0;                       
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = SwapChainBufferCount;           
        sd.OutputWindow = mhRenderWnd;                   
        sd.Windowed = true;                             
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        Exception::CheckResult(mdxgiFactory->CreateSwapChain(
            mCommandQueue.Get(),
            &sd,
            mSwapChain.GetAddressOf()
        ));
    }

    void D3DCore::CreateMSAAResource(float clearColorR, float clearColorG, float clearColorB)
    {
        D3D12_DESCRIPTOR_HEAP_DESC msaaRtvHeapDesc;
        msaaRtvHeapDesc.NumDescriptors = 1;
        msaaRtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        msaaRtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        msaaRtvHeapDesc.NodeMask = 0;
        Exception::CheckResult(md3dDevice->CreateDescriptorHeap(
            &msaaRtvHeapDesc, IID_PPV_ARGS(mMsaaRtvHeap.GetAddressOf())));

        D3D12_DESCRIPTOR_HEAP_DESC msaaDsvHeapDesc;
        msaaDsvHeapDesc.NumDescriptors = 1;
        msaaDsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        msaaDsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        msaaDsvHeapDesc.NodeMask = 0;
        Exception::CheckResult(md3dDevice->CreateDescriptorHeap(
            &msaaDsvHeapDesc, IID_PPV_ARGS(mMsaaDsvHeap.GetAddressOf())));

        D3D12_RESOURCE_DESC msaaRtvDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            mBackBufferFormat, mClientWidth, mClientHeight, 1, 1, mMSAASampleCount);

        msaaRtvDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

        D3D12_CLEAR_VALUE msaaOptimizedClearValue = {};
        msaaOptimizedClearValue.Format = mBackBufferFormat;
        float arr[4] = { clearColorR, clearColorG, clearColorB, 1.0f };
        memcpy(msaaOptimizedClearValue.Color, arr, sizeof(float) * 4);

        Exception::CheckResult(md3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &msaaRtvDesc,
            D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
            &msaaOptimizedClearValue,
            IID_PPV_ARGS(mMsaaRenderTarget.ReleaseAndGetAddressOf())));

        mMsaaRenderTarget->SetName(L"MSAA Render target");

        D3D12_RENDER_TARGET_VIEW_DESC msaaRtvViewDesc = {};
        msaaRtvViewDesc.Format = mBackBufferFormat;
        msaaRtvViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;

        md3dDevice->CreateRenderTargetView(
            mMsaaRenderTarget.Get(), &msaaRtvViewDesc,
            mMsaaRtvHeap->GetCPUDescriptorHandleForHeapStart());

        D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            mDepthStencilFormat, mClientWidth, mClientHeight,
            1, 1, mMSAASampleCount);

        depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
        depthOptimizedClearValue.Format = mDepthStencilFormat;
        depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
        depthOptimizedClearValue.DepthStencil.Stencil = 0;

        Exception::CheckResult(md3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &depthStencilDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(mMsaaDepthStencil.ReleaseAndGetAddressOf())));

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = mDepthStencilFormat;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;

        md3dDevice->CreateDepthStencilView(
            mMsaaDepthStencil.Get(), &dsvDesc,
            mMsaaDsvHeap->GetCPUDescriptorHandleForHeapStart());
    }

    void D3DCore::FlushCommandQueue()
    {
        ++mCurrentFence;

        Exception::CheckResult(mCommandQueue->Signal(
            mFence.Get(), mCurrentFence
        ));

        if (mFence->GetCompletedValue() < mCurrentFence)
        {
            HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

            Exception::CheckResult((
                mFence->SetEventOnCompletion(mCurrentFence, eventHandle)
                ));

            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        }
    }

    ID3D12Resource* D3DCore::CurrentBackBuffer()const
    {
        return mSwapChainBuffer[mCurrBackBuffer].Get();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE D3DCore::CurrentBackBufferView()const
    {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(
            mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
            mCurrBackBuffer,
            mRtvDescriptorSize);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE D3DCore::DepthStencilView()const
    {
        return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
    }

    void D3DCore::LogAdapters()
    {
        UINT i = 0;
        IDXGIAdapter* adapter = nullptr;
        std::vector<IDXGIAdapter*> adapterList;
        while (mdxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
        {
            DXGI_ADAPTER_DESC desc;
            adapter->GetDesc(&desc);

            UString text = "***Adapter: ";
            text = text + desc.Description;
            text = text + "\n";

            std::cout << text.GetC_Str() << std::endl;

            adapterList.push_back(adapter);

            ++i;
        }

        for (size_t i = 0; i < adapterList.size(); ++i)
        {
            LogAdapterOutputs(adapterList[i]);
            adapterList[i]->Release();
        }

        std::sort(resolutionRatios.begin(), resolutionRatios.end());
    }

    void D3DCore::LogAdapterOutputs(IDXGIAdapter* adapter)
    {
        UINT i = 0;
        IDXGIOutput* output = nullptr;
        while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
        {
            DXGI_OUTPUT_DESC desc;
            output->GetDesc(&desc);

            UString text = "***Output: ";
            text = text + desc.DeviceName;
            text = text + "\n";

            std::cout << text.GetStr().c_str() << std::endl;

            LogOutputDisplayModes(output, mBackBufferFormat);

            output->Release();

            ++i;
        }
    }

    void D3DCore::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
    {
        UINT count = 0;
        UINT flags = 0;
        UINT n, d;
        UString text;
        ResolutionRatio rr;

        // Call with nullptr to get list count.
        output->GetDisplayModeList(format, flags, &count, nullptr);

        std::vector<DXGI_MODE_DESC> modeList(count);
        output->GetDisplayModeList(format, flags, &count, &modeList[0]);

        for (auto& x : modeList)
        {
            n = x.RefreshRate.Numerator;
            d = x.RefreshRate.Denominator;
            text =
                "Width = " + std::to_string(x.Width) + " " +
                "Height = " + std::to_string(x.Height) + " " +
                "Refresh = " + std::to_string(n) + "/" + std::to_string(d) +
                "\n";

            std::cout << text.GetStr().c_str();

            rr = ResolutionRatio(x.Width, x.Height, n, d);

            auto it = std::find(resolutionRatios.begin(), resolutionRatios.end(), rr);
            if(it == resolutionRatios.end())
                resolutionRatios.emplace_back(x.Width, x.Height, n, d);
        }
    }

    void D3DCore::CalculateFrameStats()
    {
        static int frameCnt = 0;
        static float timeElapsed = 0.0f;

        mTimer.Tick();
        frameCnt += 1;

        if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
        {
            float fps = (float)frameCnt / (mTimer.TotalTime() - timeElapsed); 
            float mspf = 1000.0f / fps;

            UString fpsStr = "FPS:" + std::to_string((int)fps);
            UString mspfStr = "  MSPF:" + std::to_string(mspf);

            mspfStr.GetStr().resize(Min<int>(mspfStr.GetStr().size(), 12));
            frameInfo = fpsStr + mspfStr;

            frameCnt = 0;
            timeElapsed += 1.0f;
        }
    }

    bool D3DCore::IsBuild()const
    {
        return mAppBuild;
    }

    UString D3DCore::GetFrameInfo()const
    {
        return frameInfo;
    }

    void D3DCore::SetPaused(bool isPaused)
    {
        if (isPaused)
        {
            mAppPaused = true;
            mTimer.Stop();
        }
        else
        {
            mAppPaused = false;
            mTimer.Start();
        }
    }

    void D3DCore::SetScreenSize(int screenWidth, int screenHeight)
    {
        if (screenWidth != mClientWidth || screenHeight != mClientHeight)
        {
            mClientWidth = screenWidth;
            mClientHeight = screenHeight;

            OnResize();
        }
    }

    bool D3DCore::GetEnableSync()const
    {
        return enableSync;
    }

    void D3DCore::SetEnableSync(bool _enableSync)
    {
        enableSync = _enableSync;
    }

    void D3DCore::PresentSwapChain()
    {
        if (enableSync)
            mSwapChain->Present(1, 0);
        else
            mSwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);

        mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
    }

    const std::vector<ResolutionRatio>& D3DCore::GetResolutionRatios()const
    {
        return resolutionRatios;
    }

    void D3DCore::SetResolutionIndex(int index)
    {
        if (index >= resolutionRatios.size())
            Exception::ThrowException(LEVEL_Error, "Index out of range");
        if (resolutionIndex != index)
        {
            resolutionIndex = index;

            SetScreenSize(
                resolutionRatios[resolutionIndex].width,
                resolutionRatios[resolutionIndex].height);
        }
    }

    bool D3DCore::GetFillScreenState()const
    {
        return fullScreen;
    }

    void D3DCore::SetFullScreenState(bool _fullScreen)
    {
        static RECT rect;

        if (fullScreen != _fullScreen)
        {
            fullScreen = _fullScreen;

            if (fullScreen)
            {
                GetWindowRect(mhRenderWnd, &rect);
                SetFullScreen(mhRenderWnd);
            }
            else
            {
                SetWindowed(mhRenderWnd);
                MoveWindow(mhRenderWnd, rect.left, rect.top, 
                    rect.right - rect.left, rect.bottom - rect.top, false);
            }
        }
    }
}
#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <D3d12SDKLayers.h>
#include <d3dcompiler.h>
#include "d3dx12.h"
#include <dxgi1_4.h>
#include <wrl.h>

#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Timer.h"
#include "AuxiliaryBase.h"
#include "AuxiliaryD3D.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx12.h>

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"D3D12.lib")
#pragma comment(lib,"dxgi.lib")

using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;

namespace Rendering
{        
    class D3DCore
    {
    public:
        D3DCore() {}

        D3DCore(const D3DCore& rhs) = delete;            
        D3DCore& operator=(const D3DCore& rhs) = delete;  

        virtual ~D3DCore()                               
        {
            if (md3dDevice != nullptr)       
                FlushCommandQueue();
        }

        virtual void Build(HINSTANCE _hInstance, HWND renderHWindow);
        
        virtual void Draw();

    public:
        HINSTANCE AppInst();

        HWND      RenderWnd();

        float     AspectRatio();

        bool GetMsaaState();

        int GetMsaaSampleCount()const;

        int GetMsaaMaxSampleCount()const;

        void SetMsaaState(bool enable);

        void SetMSAASampleCount(int sampleCount);

        bool GetEnableSync()const;
        void SetEnableSync(bool _enableSync);

        const std::vector<ResolutionRatio>& GetResolutionRatios()const;
        void SetResolutionIndex(int index);

        bool GetFillScreenState()const;
        void SetFullScreenState(bool _fullScreen);

        void CalculateFrameStats();

        UString GetFrameInfo()const;

        bool IsBuild()const;

        void SetPaused(bool isPaused);

        virtual void SetScreenSize(int screenWidth, int screenHeight);

    protected:
        virtual bool Initialize();

        virtual void CreateRtvAndDsvDescriptorHeaps();

        virtual void OnResize();

        void PresentSwapChain();

        bool InitDirect3D();

        void CreateCommandObjects();

        void CreateSwapChain();

        void ChangeMSAAClearValue(float clearColorR = 0.0f, float clearColorG = 0.0f, float clearColorB = 0.0f);

        void CreateMSAAResource(float clearColorR = 0.0f, float clearColorG = 0.0f, float clearColorB = 0.0f);

        void FlushCommandQueue();

        void LogAdapters();

        void LogAdapterOutputs(IDXGIAdapter* adapter);

        void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

        ID3D12Resource* CurrentBackBuffer()const;

        D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;

        D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

    protected:
        Timer mTimer; 

        bool mAppBuild = false;
        bool mAppPaused = true;

        float backColor[3] = { 0.0f, 0.0f, 0.0f };               // backgroud buffer clear value

        HINSTANCE mhAppInst = nullptr; 
        HWND      mhRenderWnd = nullptr; 

        bool      mMsaaState = false;                            // enable MSAA state
        UINT      mMsaaQuality = 0;      

        Microsoft::WRL::ComPtr<IDXGIFactory4>   mdxgiFactory; 
        Microsoft::WRL::ComPtr<IDXGISwapChain>  mSwapChain; 
        Microsoft::WRL::ComPtr<ID3D12Device>    md3dDevice;    

        Microsoft::WRL::ComPtr<ID3D12Fence> mFence;        
        UINT64 mCurrentFence = 0;                          

        Microsoft::WRL::ComPtr<ID3D12CommandQueue>        mCommandQueue;           
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator>    mCommandAllocator;   
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;     

        static const int SwapChainBufferCount = 2;  
        int mCurrBackBuffer = 0;                   
        Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];  
        Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;                     

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;      
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;      

        Microsoft::WRL::ComPtr<ID3D12Resource> mMsaaRenderTarget;  
        Microsoft::WRL::ComPtr<ID3D12Resource> mMsaaDepthStencil;   

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mMsaaRtvHeap;  
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mMsaaDsvHeap;  

        D3D12_VIEWPORT mScreenViewport;     
        D3D12_RECT mScissorRect;            

        UINT mRtvDescriptorSize = 0;        
        UINT mDsvDescriptorSize = 0;       
        UINT mCbvSrvUavDescriptorSize = 0;  

        D3D_DRIVER_TYPE md3dDriverType =  D3D_DRIVER_TYPE_HARDWARE;   

        DXGI_FORMAT mBackBufferFormat =   DXGI_FORMAT_R8G8B8A8_UNORM;         
        DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;    

        int mClientWidth;                                                  
        int mClientHeight;

        int mMSAASampleCount = 0;                                           // MSAA ues value of sample count
        int mMSAAMaxSampleCount = 0;                                        // MSAA max value of sample count

        bool fullScreen = false;
        bool enableSync = false;
        int resolutionIndex = -1;
        std::vector<ResolutionRatio> resolutionRatios;

        UString frameInfo;
    };
}

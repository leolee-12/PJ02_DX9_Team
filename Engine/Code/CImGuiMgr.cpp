#include "CImGuiMgr.h"

#include <Windows.h>

#include <ImGui_Define.h>

using namespace Engine;
using namespace std;

IMPLEMENT_SINGLETON(CImGuiMgr)

static LPDIRECT3D9              g_pD3D = nullptr;

static D3DPRESENT_PARAMETERS    g_d3dpp = {};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

CImGuiMgr::CImGuiMgr()
{
}

CImGuiMgr::~CImGuiMgr()
{
    Free();
}

void CImGuiMgr::Free()
{
}


void CImGuiMgr::ImGui_Setup(HWND hWnd, LPDIRECT3DDEVICE9 pDevice, WNDCLASSEXW& wndclass)
{
    m_pGraphicDev = pDevice;
    m_pGraphicDev->AddRef();
    m_hWnd = hWnd;
    wc = wndclass;

    // CRITICAL: D3DPRESENT_PARAMETERS 초기화
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));

    // 현재 디바이스의 Present Parameters 가져오기
    IDirect3DSwapChain9* pSwapChain = nullptr;
    if (SUCCEEDED(m_pGraphicDev->GetSwapChain(0, &pSwapChain)))
    {
        pSwapChain->GetPresentParameters(&g_d3dpp);
        pSwapChain->Release();
    }
    else
    {
        // 기본값으로 초기화
        RECT rect;
        GetClientRect(hWnd, &rect);

        g_d3dpp.Windowed = TRUE;
        g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
        g_d3dpp.BackBufferWidth = rect.right - rect.left;
        g_d3dpp.BackBufferHeight = rect.bottom - rect.top;
        g_d3dpp.EnableAutoDepthStencil = TRUE;
        g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
        g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX9_Init(m_pGraphicDev);

    // 폰트설정

    io.Fonts->AddFontFromFileTTF("../Font/arial.ttf", 30.0f);
}

void CImGuiMgr::ImGui_Tick()
{
    ImGuiIO& io = ImGui::GetIO();

    // Handle lost D3D9 device
    if (g_DeviceLost)
    {
        HRESULT hr = m_pGraphicDev->TestCooperativeLevel();
        if (hr == D3DERR_DEVICELOST)
        {
            ::Sleep(10);
            return;
        }
        if (hr == D3DERR_DEVICENOTRESET)
            ResetDevice();
        g_DeviceLost = false;
    }

    // Handle window resize (we don't resize directly in the WM_SIZE handler)
    if (m_ResizeWidth != 0 && m_ResizeHeight != 0)
    {
        g_d3dpp.BackBufferWidth = m_ResizeWidth;
        g_d3dpp.BackBufferHeight = m_ResizeHeight;
        m_ResizeWidth = m_ResizeHeight = 0;
        ResetDevice();
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (m_show_demo_window)
        ImGui::ShowDemoWindow(&m_show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &m_show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &m_show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&m_clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (m_show_another_window)
    {
        ImGui::Begin("Another Window", &m_show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            m_show_another_window = false;
        ImGui::End();
    }
}

void CImGuiMgr::ImGui_Render()
{
    // Rendering
    ImGui::EndFrame();
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(m_clear_color.x * m_clear_color.w * 255.0f), (int)(m_clear_color.y * m_clear_color.w * 255.0f), (int)(m_clear_color.z * m_clear_color.w * 255.0f), (int)(m_clear_color.w * 255.0f));
    m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
    if (m_pGraphicDev->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        m_pGraphicDev->EndScene();
    }
    HRESULT result = m_pGraphicDev->Present(nullptr, nullptr, nullptr, nullptr);
    if (result == D3DERR_DEVICELOST)
        g_DeviceLost = true;
}

void CImGuiMgr::ImGui_Shutdown()
{
   // Free();

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}


// ==========================
//	ImGui helper functions
// ==========================

bool CImGuiMgr::CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &m_pGraphicDev) < 0)
        return false;

    return true;
}

void CImGuiMgr::CleanupDeviceD3D()
{
    if (m_pGraphicDev) { m_pGraphicDev->Release(); m_pGraphicDev = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void CImGuiMgr::ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = m_pGraphicDev->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

LRESULT __stdcall CImGuiMgr::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        m_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        m_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);

}

void CImGuiMgr::Set_Resize(UINT width, UINT height)
{
    m_ResizeWidth = width;
    m_ResizeHeight = height;
}

#include "pch.h"
#include "CImGuiManager.h"


CImGuiManager* CImGuiManager::Instance = nullptr;
LPDIRECT3DDEVICE9 CImGuiManager::m_pGraphicDev = nullptr;

static LPDIRECT3D9              g_pD3D = nullptr;

static D3DPRESENT_PARAMETERS    g_d3dpp = {};


// ==========================
//	CImGuiManager functions
// ==========================

CImGuiManager::CImGuiManager()
{
}

CImGuiManager::~CImGuiManager()
{

}

void CImGuiManager::ImGui_Setup(HWND hWnd, LPDIRECT3DDEVICE9 pDevice)
{
    m_pGraphicDev = pDevice;
    m_pGraphicDev->AddRef();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX9_Init(m_pGraphicDev);
}

void CImGuiManager::ImGui_Tick()
{
    // Handle window resize (we don't resize directly in the WM_SIZE handler)
    if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
    {
        g_d3dpp.BackBufferWidth = g_ResizeWidth;
        g_d3dpp.BackBufferHeight = g_ResizeHeight;
        g_ResizeWidth = g_ResizeHeight = 0;
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
        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::SliderFloat("float", &m_f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&m_clear_color); // Edit 3 floats as a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            m_counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", m_counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
}

void CImGuiManager::ImGui_Render()
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

void CImGuiManager::ImGui_Shutdown()
{
    Free();

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

unsigned long CImGuiManager::Free()
{
    unsigned long		dwRefCnt = 0;

    if (nullptr != m_pGraphicDev)
    {
        dwRefCnt = m_pGraphicDev->Release();

        if (0 == dwRefCnt)
            m_pGraphicDev = NULL;
    }

    return dwRefCnt;
}


// ==========================
//	ImGui helper functions
// ==========================

bool CImGuiManager::CreateDeviceD3D(HWND hWnd)
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

void CImGuiManager::CleanupDeviceD3D()
{
    if (m_pGraphicDev) { m_pGraphicDev->Release(); m_pGraphicDev = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void CImGuiManager::ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = m_pGraphicDev->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}
//
//// Forward declare message handler from imgui_impl_win32.cpp
//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//
//// Win32 message handler
//// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
//// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
//// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
//// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
//LRESULT WINAPI CImGuiManager::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
//        return true;
//
//    switch (msg)
//    {
//    case WM_SIZE:
//        if (wParam == SIZE_MINIMIZED)
//            return 0;
//        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
//        g_ResizeHeight = (UINT)HIWORD(lParam);
//        return 0;
//    case WM_SYSCOMMAND:
//        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
//            return 0;
//        break;
//    case WM_DESTROY:
//        ::PostQuitMessage(0);
//        return 0;
//    }
//    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
//}

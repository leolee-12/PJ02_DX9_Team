#include "CImGuiMgr.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#include "imgui.h"
#include <d3d9.h>
#include <d3dx9.h>

#include "ImGuizmo.h"

#include "Engine_Define.h"

using namespace Engine;
using namespace std;

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

float CImGuiMgr::ImGui_Init()
{
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    return main_scale;
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
    io.Fonts->AddFontFromFileTTF("../Font/arial.ttf", m_font_size);
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

    ImGuizmo::BeginFrame();

    ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_Always);
    Tick_EditorWindow();

    Additonal_Tick();
}

void CImGuiMgr::ImGui_Render()
{
    // Rendering
    ImGui::EndFrame();
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

    Render_Grid();

    Addtional_Render();

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    m_pGraphicDev->EndScene();

    HRESULT result = m_pGraphicDev->Present(nullptr, nullptr, nullptr, nullptr);
    if (result == D3DERR_DEVICELOST)
        g_DeviceLost = true;
}

void CImGuiMgr::ImGui_Shutdown()
{
    // Free();
    Safe_Release(m_pGraphicDev);

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void CImGuiMgr::Tick_EditorWindow()
{
    ImGuiIO& io = ImGui::GetIO();
}

void CImGuiMgr::Render_Grid()
{
    ImGuiIO& io = ImGui::GetIO();

    // 전체 화면 영역 설정
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    // 카메라 행렬 (예시 - 실제 카메라 행렬로 교체 필요)

    float cameraView[16];
    float cameraProjection[16];

    _matrix matView, matProj;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

    for (int i = 0; i < 4; ++i)
    {
        cameraView[4 * i] = matView.m[i][0];
        cameraView[4 * i + 1] = matView.m[i][1];
        cameraView[4 * i + 2] = matView.m[i][2];
        cameraView[4 * i + 3] = matView.m[i][3];

        cameraProjection[4 * i] = matProj.m[i][0];
        cameraProjection[4 * i + 1] = matProj.m[i][1];
        cameraProjection[4 * i + 2] = matProj.m[i][2];
        cameraProjection[4 * i + 3] = matProj.m[i][3];
    }

    _matrix matWorld;
    D3DXMatrixInverse(&matWorld, nullptr, &matView);

    float cameraX = matWorld.m[3][0];
    float cameraZ = matWorld.m[3][2];

    // 격자 크기 단위로 반내림 (격자에 정렬)
    float gridPosX = floorf(cameraX / m_grid_size) * m_grid_size;
    float gridPosZ = floorf(cameraZ / m_grid_size) * m_grid_size;

    // 카메라 X, Z 위치로 이동하는 행렬 (Y는 0으로 고정 - 바닥에 격자)
    float gridPositionMatrix[16] = {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f  // 마지막 행에 위치 정보
    };

    // 격자 그리기
    // 파라미터: view 행렬, projection 행렬, 중심 행렬, 격자 크기
    ImGuizmo::DrawGrid(cameraView, cameraProjection, gridPositionMatrix, m_grid_size);
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

void CImGuiMgr::ImGuiWindowBegin(const char* windowName)
{
    ImGui::Begin(windowName);
}

void CImGuiMgr::ImGuiWindowEnd()
{
    ImGui::End();
}

void CImGuiMgr::ImGuiText(const char* text)
{
    ImGui::Text(text);
}

void CImGuiMgr::ImGuiDragFontSize(const char* text)
{
    ImGuiStyle& style = ImGui::GetStyle();

    if (ImGui::DragFloat(text, &style.FontSizeBase, 0.20f, 5.0f, 100.f, "%.0f"))
        style._NextFrameFontSizeBase = style.FontSizeBase;
}

void CImGuiMgr::ImGuiComboBox(const char* text, int* types, const char* type_str[], int size)
{
    ImGui::Combo(text, types, type_str, size);
}

bool CImGuiMgr::ImGuiButton(const char* text)
{
    return ImGui::Button(text);
}

void CImGuiMgr::ImGuiSameLine()
{
    ImGui::SameLine(0.0f, 0.0f);
}

void CImGuiMgr::ImGuiMouseText()
{
    // 마우스 위치 표시
    ImVec2 mousePos = ImGui::GetMousePos();
    ImGui::Text("Mouse Position: (%.1f, %.1f)", mousePos.x, mousePos.y);

}

#pragma once
#include "CBase.h"
#include "Engine_Macro.h"

#include <d3d9.h>
#include <d3dx9.h>

#include "imgui.h"

//#include "imgui_demo.cpp"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
//#include "imgui_internal.h"

 /* -------------------------------------------
    CImGuiManager
 
    ImGui을 이식한 맵툴 매니저입니다.
 -------------------------------------------- */

BEGIN(Engine)

class ENGINE_DLL CImGuiMgr : public CBase
{
    DECLARE_SINGLETON(CImGuiMgr)

private:
    explicit  CImGuiMgr();
    virtual ~CImGuiMgr();

private:
    virtual void Free();

public:
    void ImGui_Setup(HWND hWnd, LPDIRECT3DDEVICE9 pDevice, WNDCLASSEXW& wndclass);
    void ImGui_Tick();
    void ImGui_Render();
    void ImGui_Shutdown();

private:
    LPDIRECT3DDEVICE9 m_pGraphicDev = nullptr;
    HWND m_hWnd;

public:
    /* imgui helper function */
    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void ResetDevice();
    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    // ImGui state variables
    bool m_show_demo_window = true;
    bool m_show_another_window = false;
    ImVec4 m_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool g_DeviceLost = false;
    int m_counter = 0;
    float m_f = 0.0f;

public:
    void Set_Resize(UINT width, UINT height);
    // ImGui
    UINT                     m_ResizeWidth = 0, m_ResizeHeight = 0;
    WNDCLASSEXW wc;

};

END
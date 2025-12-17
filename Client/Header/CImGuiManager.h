#pragma once

#include <windows.h>
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <d3d9types.h>
#include <tchar.h>
#include "ImGui_Define.h"
#include "Engine_Macro.h"

class CImGuiManager
{
public:
    static CImGuiManager* Instance;

public:
    static CImGuiManager* GetInstance()
    {
        if (nullptr == Instance)
            Instance = new CImGuiManager;
        return Instance;
    }
    static void DestroyInstance()
    {
        if (nullptr != Instance)
        {
            delete Instance;
            Instance = nullptr;
        }
    }

public:
    void ImGui_Setup(HWND hWnd, LPDIRECT3DDEVICE9 pDevice);
    void ImGui_Tick();
    void ImGui_Render();
    void ImGui_Shutdown();

    unsigned long Free();

private:
    CImGuiManager();
    ~CImGuiManager();

private:
    static LPDIRECT3DDEVICE9 m_pGraphicDev;

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
};

#pragma once
#include "CBase.h"
#include "Engine_Macro.h"
#include "Edit_Define.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <functional>

/* -------------------------------------------
   CImGuiManager

   ImGui을 이식한 맵툴 매니저입니다.
-------------------------------------------- */

BEGIN(Engine)

class ENGINE_DLL CImGuiMgr : public CBase
{

public:
    CImGuiMgr();
    virtual ~CImGuiMgr();

private:
    virtual void Free();

public:
    float ImGui_Init();
    void ImGui_Setup(HWND hWnd, LPDIRECT3DDEVICE9 pDevice, WNDCLASSEXW& wndclass);
    void ImGui_Tick();
    void ImGui_Render();
    void ImGui_Shutdown();

    virtual void Additonal_Tick() {}
    virtual void Addtional_Render() {}

public:
    virtual void Tick_EditorWindow();

    void Render_Grid();
    void Set_Resize(UINT width, UINT height);


protected:
    // imgui 관련 랩핑 함수
    void ImGuiWindowBegin(const char* windowName);
    void ImGuiWindowEnd();

    void ImGuiText(const char* text);
    void ImGuiDragFontSize(const char* text);
    void ImGuiComboBox(const char* text, int* types, const char* type_str[], int size);
    bool ImGuiButton(const char* text);
    void ImGuiSameLine();
    void ImGuiMouseText();

public:
    LPDIRECT3DDEVICE9 m_pGraphicDev = nullptr;
    HWND m_hWnd;

public:
    /* imgui helper function */
    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void ResetDevice();
    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    int m_counter = 0;
    OBJECT_TYPE objectType = (OBJECT_TYPE)(-1);
    OBJECT_ID objectId = OBJECT_TEST;
    LIGHT_ID lightId = LIGHT_TEST;

    float m_font_size = 30.f;
    float m_grid_size = 1000.f;

    bool g_DeviceLost = false;
    float m_f = 0.0f;

protected:
    UINT                     m_ResizeWidth = 0, m_ResizeHeight = 0;
    WNDCLASSEXW wc;
};

END
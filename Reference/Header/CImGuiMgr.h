#pragma once
#include "CBase.h"
#include "Engine_Macro.h"
#include "Edit_Define.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <functional>

class CGameObject;
class ImTextureData;
class CCamera;

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
    float ImGui_Init();
    void ImGui_Setup(HWND hWnd, LPDIRECT3DDEVICE9 pDevice, WNDCLASSEXW& wndclass);
    void ImGui_Tick();
    void ImGui_Render();
    void ImGui_Shutdown();

public:
    void Tick_EditorWindow();
    void Tick_ObjectWindow(CGameObject* pObject);

    void Render_Grid();

    void Set_Resize(UINT width, UINT height);

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
    int m_counter = 0;
    OBJECT_TYPE objectType = (OBJECT_TYPE)(-1);
    OBJECT_ID objectId = OBJECT_TEST;
    LIGHT_ID lightId = LIGHT_TEST;

    ImTextureData* TexData;            // Latest texture.
    float m_font_size = 30.f;

    float m_grid_size = 1000.f;

    bool g_DeviceLost = false;
    float m_f = 0.0f;
    UINT                     m_ResizeWidth = 0, m_ResizeHeight = 0;
    WNDCLASSEXW wc;
};

END
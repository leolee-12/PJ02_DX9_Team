#include "pch.h"
#include "CImGuiManager.h"


CImGuiManager* CImGuiManager::Instance = nullptr;
LPDIRECT3DDEVICE9 CImGuiManager::m_pGraphicDev = nullptr;

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

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among loaded ones.
    // - If your file paths are relative and you want to load them from a file system you can change the ImFileRead function.
    // - The synthetic best fit scale factor of 1.0f is used in ImFontAtlas::AddFontDefault().
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Calibri.ttf", 24.0f, nullptr, io.Fonts->GetGlyphRangesKorean());
    //ImFont* font = io.Fonts->AddFontFromFileTTF("res/Roboto-Medium.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);
}

void CImGuiManager::ImGui_Tick()
{
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

void CImGuiManager::ImGui_Render(LPDIRECT3DDEVICE9 pDevice)
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

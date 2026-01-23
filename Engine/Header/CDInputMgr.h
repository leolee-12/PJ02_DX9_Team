#pragma once

#include "Engine_Define.h"
#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CDInputMgr : public CBase
{
	DECLARE_SINGLETON(CDInputMgr)

private:
	explicit CDInputMgr(void);
	virtual ~CDInputMgr(void);

public:
	_byte	Get_DIKeyState(_ubyte byKeyID)
	{
		return m_byKeyState[byKeyID];
	}

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse)
	{
		return m_tMouseState.rgbButtons[eMouse];
	}

	// 현재 마우스의 특정 축 좌표값 반환
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return *(((_long*)&m_tMouseState) + eMouseState);
	}

	_bool	Key_Down(_ubyte byKeyID)
	{
		if (!(m_byPreKeyState[byKeyID] & 0x80) && (m_byKeyState[byKeyID] & 0x80))
		{
			return true;
		}
		return false;
	}

	_bool Key_Pressing(_ubyte byKeyID)
	{
		if (m_byKeyState[byKeyID] & 0x80)
		{
			return true;
		}
		return false;
	}

	_bool Key_Up(_ubyte byKeyID)
	{
		if ((m_byPreKeyState[byKeyID] & 0x80) && !(m_byKeyState[byKeyID] & 0x80))
		{
			return true;
		}
		return false;
	}

	// 마우스 버튼 입력 함수
	_bool Mouse_Down(MOUSEKEYSTATE eMouse)
	{
		if (!(m_tPreMouseState.rgbButtons[eMouse] & 0x80) && (m_tMouseState.rgbButtons[eMouse] & 0x80))
		{
			return true;
		}
		return false;
	}

	_bool Mouse_Pressing(MOUSEKEYSTATE eMouse)
	{
		if (m_tMouseState.rgbButtons[eMouse] & 0x80)
		{
			return true;
		}
		return false;
	}

	_bool Mouse_Up(MOUSEKEYSTATE eMouse)
	{
		if ((m_tPreMouseState.rgbButtons[eMouse] & 0x80) && !(m_tMouseState.rgbButtons[eMouse] & 0x80))
		{
			return true;
		}
		return false;
	}

public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	_byte					m_byKeyState[256];		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	_byte					m_byPreKeyState[256];
	DIMOUSESTATE			m_tMouseState;
	DIMOUSESTATE			m_tPreMouseState;

public:
	virtual void	Free(void);

};
END


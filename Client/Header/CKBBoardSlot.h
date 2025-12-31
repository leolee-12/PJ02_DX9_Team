#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

// 보드 레이아웃 상수
namespace KBBoard
{
	// 슬롯 렌더링 크기 (실제 화면에 보이는 크기)
	const _float SLOT_RENDER_WIDTH = 90.f;
	const _float SLOT_RENDER_HEIGHT = 60.f;

	// 슬롯 간격 (0이면 딱 붙음)
	const _float SLOT_GAP_X = 25.f;			// X축 간격 (열 사이)
	const _float SLOT_GAP_Y = 5.f;			// Y축 간격 (행 사이)

	// 슬롯 스텝 (렌더링 크기 + 간격)
	const _float SLOT_STEP_X = SLOT_RENDER_WIDTH + SLOT_GAP_X;
	const _float SLOT_STEP_Y = SLOT_RENDER_HEIGHT + SLOT_GAP_Y;

	// 보드 위치
	const _float BOARD_CENTER_X = 0.f;		// 보드 중앙 X (화면 중심)
	const _float NPC_BOARD_Y = 120.f;		// NPC 보드 중앙 Y
	const _float PLAYER_BOARD_Y = -120.f;	// 플레이어 보드 중앙 Y
}

// 보드의 한 칸을 나타내는 UI 클래스
class CKBBoardSlot :
	public CUi
{
private:
	explicit CKBBoardSlot(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CKBBoardSlot();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	// 슬롯 정보 접근자
	_int			Get_Owner() const { return m_iOwner; }		// 0: 플레이어, 1: NPC
	_int			Get_Col() const { return m_iCol; }			// 열 (0~2)
	_int			Get_Row() const { return m_iRow; }			// 행 (0~2)

public:
	// iOwner: 0=플레이어, 1=NPC / iCol: 열(0~2) / iRow: 행(0~2)
	static CKBBoardSlot* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iOwner, _int iCol, _int iRow);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex*			m_pBufferCom;
	CTransform*		m_pTransformCom;
	CTexture*		m_pTextureCom;

	// 슬롯 정보
	_int			m_iOwner;		// 0: 플레이어, 1: NPC
	_int			m_iCol;			// 열 (0~2)
	_int			m_iRow;			// 행 (0~2)
};

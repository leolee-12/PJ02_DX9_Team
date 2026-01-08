#pragma once
#include "CScene.h"

class CKBTab;
class CKBDice;
class CKBBoardBack;
class CKBMask;

class CKnuckleBone : public CScene
{
public:
	enum KBSTATE { KB_TITLE, KB_TUTO, KB_MAIN, KB_END };
	enum KBTITLEOPTION { KBT_PLAY, KBT_HOWTOPLAY, KBT_EXIT, KBT_END };

	// 메인 게임 내 상태
	enum MAINSTATE {
		MS_ROLL,		// 주사위 굴리는 중
		MS_SELECT,		// 열 선택 중
		MS_PLACE,		// 주사위 배치 중 (이동 애니메이션)
		MS_RESULT,		// 게임 결과 표시
		MS_END
	};
private:
	explicit CKnuckleBone(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CKnuckleBone();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag) { return S_OK; }
	HRESULT			Ready_Tutorial_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_Title_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_Main_Layer(const _tchar* pLayerTag);

	HRESULT			Ready_Light();
	void			Render_Font_Tutorial();
	void			Render_Font_Title();
	void			Render_Font_Main();

	void			State_machine();
	void			Key_Input_KB();

	// 메인 게임 로직
	_bool			Update_MainGame(const _float& fTimeDelta);	// true 반환 시 씬 전환됨, 즉시 탈출 필요
	void			Start_Roll();						// 주사위 굴리기 시작
	void			Place_Dice(_int iOwner, _int iCol);	// 주사위 배치
	_int			Find_EmptyRow(_int iOwner, _int iCol);	// 빈 행 찾기 (-1이면 꽉 참)
	_vec3			Get_SlotPosition(_int iOwner, _int iCol, _int iRow);	// 슬롯 위치 계산

	// 점수 계산
	_int			Calc_ColumnScore(_int iOwner, _int iCol);	// 열 점수 계산
	_int			Calc_TotalScore(_int iOwner);				// 전체 점수 계산

	// NPC AI
	_int			NPC_Select_Column();						// NPC 열 선택 AI
	void			Switch_Turn();								// 턴 교대

	// 상대 주사위 제거
	void			Remove_OpponentDice(_int iOwner, _int iCol, _int iDiceValue);	// 상대 같은 열 같은 눈 제거
	void			Compact_Column(_int iOwner, _int iCol);							// 열 내 빈 칸 정렬

	// 게임 종료 판정
	_bool			Check_GameEnd();				// 게임 종료 여부 (한 쪽 9칸 채움)
	_int			Get_Winner();					// 승자 반환 (0=플레이어, 1=NPC, -1=무승부)

	HRESULT			Ready_MainGame();				// 게임 초기화

public:
	static CKnuckleBone* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();

private:
	KBSTATE		m_eCurKBState;
	KBSTATE		m_ePreKBState;
	wstring		m_strLayerTag;

	CKBTab*		  m_pTitleTab;
	CKBBoardBack* m_pBoardBack;
	CKBMask*	  m_pKBMask;

	KBTITLEOPTION m_eTitleOption;

	CKBDice*	m_pCurDice;

	// 보드 데이터: [소유자][열][행] - 0=빈칸, 1~6=주사위눈
	// 소유자: 0=플레이어, 1=NPC
	_int		m_iBoard[2][3][3];

	// 보드 위 주사위 객체 포인터: [소유자][열][행]
	CKBDice*	m_pBoardDice[2][3][3];

	// 메인 게임 상태
	MAINSTATE	m_eMainState;
	_int		m_iSelectedCol;		// 선택된 열 (0~2)
	_int		m_iCurTurn;			// 현재 턴 (0=플레이어, 1=NPC)

	// NPC 딜레이
	_float		m_fNPCThinkTime;	// NPC 생각 시간 누적

	// 게임 결과
	_float		m_fResultTime;		// 결과 표시 시간 누적
	_int		m_iWinner;			// 승자 (0=플레이어, 1=NPC, -1=무승부)

	// 보드선택창 ShowTime
	_float		m_fShowTime;
};


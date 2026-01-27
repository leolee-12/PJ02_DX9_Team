#pragma once
#include "CMonster.h"

	namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

	// ==========================
	//	CNode : 몬스터 마디 구현용 클래스
	//  - 독립적 생성X, 몬스터 클래스에서 소유 및 관리
	// ==========================

class CNode : public CGameObject
{
public:
	enum USERID { MONSTER_N2, MONSTER_B1, MONSTER_B2, USERID_END };

private:
	explicit	CNode(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CNode(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CNode(const CNode& rhs);
	virtual		~CNode();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject) {};

	void				Set_NodeScale(const _vec3& vScale);
	void				Set_NodePos(const _vec3& vPos);
	void				Set_NodeDir(const _vec3& vDir) { D3DXVec3Normalize(&m_vDir, &vDir); }
	void				Set_Depth(const _float& fDepth) { m_fDepth = fDepth; }
	const _vec3&		Get_NodePos() { return m_vPos; }
	const _vec3&		Get_NodeDir() { return m_vDir; }

	void				Set_UserID(USERID eID) { m_eUserID = eID; }
	void				Set_AcmlTime(const _float& fTime) { m_fAccTime = fTime; }
	void				Set_Material();
	void				Reset_Material();
	void				Switch_UseMaterial() { m_bUseMtrl = !m_bUseMtrl; }

private:
	HRESULT				Add_Component();

	void				Ready_Variable();
	void				Move_Frame(const _float& fTimeDelta);
	void				Set_Texture();

private:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;

	_vec3				m_vPos;
	_vec3				m_vDir;

	// 스프라이트 관련
	_float				m_fFrame;
	_float				m_fFrameEnd;
	_float				m_fFrameSpeed;
	_matrix				m_matTex;
	wstring				m_strProtoTexKey;

	// 패턴 관련
	USERID			m_eUserID;
	_float			m_fAccTime;
	_bool			m_bMtrl = false;
	_bool			m_bUseMtrl = false;

public:
	static CNode* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, const _tchar* pProtoTexKey);

private:
	virtual void		Free();
};

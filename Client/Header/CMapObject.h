#pragma once
#include "CGameObject.h"
#include "Engine_Struct.h"
#include <string>

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

enum PLACEMENT_TYPE
{
	PLACEMENT_STANDING = 0,	// �������� ���ִ� ��ü
	PLACEMENT_FLOOR = 1		// �ٴڿ� ���� ��ü
};

/* ===================================================
	CMapObject

	�������� �ϴ� �� ������Ʈ
 =====================================================*/

class CMapObject : public CGameObject
{
private:
	explicit CMapObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMapObject(const CMapObject& rhs);
	virtual ~CMapObject();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();

public:
	void	Set_ObjectData(const Engine::OBJECTDATA& objData);

private:
	HRESULT	Add_Component();

private:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;

	std::string			m_strCategory;
	_int				m_iTextureIndex;
	_float				m_fScale;
	PLACEMENT_TYPE		m_ePlacement;

	_uint				m_iTextureWidth;
	_uint				m_iTextureHeight;

public:
	static CMapObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, const Engine::OBJECTDATA& objData);

private:
	virtual void Free();
};

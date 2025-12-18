#pragma once
#include "CComponent.h"
#include "Engine_Define.h"

// ==========================
//	CTextureSet : 상태 별 텍스처 이미지를 총괄하기 위한 컴포넌트
// - 텍스처 컴객체 벡터를 해시맵으로 관리
// -> 빠른 접근 및 문자열 기반 탐색을 위해 해시맵 사용
// -> 정렬을 하지 않으며, 해시 연산이 매 프레임 일어나는 것이 아니므로 부하 적음 (상태가 변하는 시점에만 발생)
// -> 스프라이트 시작은 0, 끝은 벡터.size()로 알 수 있어, 객체마다 멤버변수로 관리할 필요가 없음
// 
// - 상태를 wstring으로 받아 key값으로 활용
// -> tchar* 형을 키값으로 쓰는것보다 안전 (주소값 비교에 의한 비정상 동작 방지)
// 
// * 주의사항
// - 수정 시 map컨테이너 접근이 필요한 경우 반복자 사용, []연산자는 대입 이외에는 지양(잘못된 요소 삽입 위험)
// - Texture 삽입을 위해 먼저 vector<TEXINFO>를 구성한 다음 매개변수로 넣어줄 것
// ==========================




BEGIN(Engine)

class ENGINE_DLL CTextureSet : public CComponent
{
public:
	typedef struct tagTextureInfo
	{
		wstring			strState;
		const _tchar*	pPath;
		_uint			iCnt;

		tagTextureInfo() : strState(L""), pPath(nullptr), iCnt(0) {}
		tagTextureInfo(wstring _strState, const _tchar* _pPath, _uint _iCnt)
			: strState(_strState), pPath(_pPath), iCnt(_iCnt) {}
	}TEXINFO;

private:
	explicit CTextureSet();
	explicit CTextureSet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTextureSet(const CTextureSet& rhs);
	virtual ~CTextureSet();

public:
	virtual HRESULT Ready_Texture(TEXTUREID eID, const vector<TEXINFO>& vecTexInfo);
	HRESULT			Add_Texture(TEXTUREID eID, const TEXINFO& tTexInfo);
	void			Set_Texture(wstring strState, const _uint& iIndex = 0);

	_uint			Get_TextureEnd(wstring strState);

private:
	unordered_map<wstring, vector<IDirect3DBaseTexture9*>>		m_mapTexture;

public:
	static CTextureSet* Create(LPDIRECT3DDEVICE9 pGraphicDev, TEXTUREID eID, vector<TEXINFO>& vecTexInfo);
	virtual CComponent* Clone();

private:
	virtual void Free();

};


END

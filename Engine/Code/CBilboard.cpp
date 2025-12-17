#include "CBilboard.h"

#include "CTerrainTex.h"
#include "CTransform.h"

CBilboard::CBilboard(LPDIRECT3DDEVICE9 pGraphicDev)
    : CComponent(pGraphicDev)
{


}

CBilboard::CBilboard(const CBilboard& rhs)
    : CComponent(rhs)
{
}

CBilboard::~CBilboard()
{
}

HRESULT CBilboard::Ready_Bilboard()
{
    return S_OK;
}



CBilboard* CBilboard::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CBilboard* pBilboard = new CBilboard(pGraphicDev);

    if (FAILED(pBilboard->Ready_Bilboard()))
    {
        Safe_Release(pBilboard);
        MSG_BOX("Calculator Create Failed");
        return nullptr;
    }

    return pBilboard;
}

CComponent* CBilboard::Clone()
{
    return new CBilboard(*this);
}

void CBilboard::Free()
{
    CComponent::Free();
}

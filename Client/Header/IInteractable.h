#pragma once
#include "Engine_Define.h"

class IInteractable
{
public:
	virtual ~IInteractable() = default;

	virtual void	Add_WorkGauge(_float fWork)	PURE;
	virtual _float	Get_WorkGauge() const		PURE;
	virtual _bool	Is_WorkComplete() const		PURE;
	virtual _vec3*	Get_WorkPos(_vec3* pWorkPos) const	PURE;
};

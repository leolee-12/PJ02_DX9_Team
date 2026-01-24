CBase::CBase() : m_dwRefCnt(1)
{
}

CBase::~CBase()
{
}

unsigned long CBase::AddRef()
{
    return ++m_dwRefCnt;
}

unsigned long CBase::Release()
{
    if (m_dwRefCnt == 0)
        return 0;

    unsigned long dwRef = --m_dwRefCnt;

    if (dwRef == 0)
    {
        Free();
        delete this;
        return 0;
    }

    return dwRef;
}

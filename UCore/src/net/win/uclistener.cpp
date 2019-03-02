#include "ucnetopt.h"
#include "ucnetconfig.h"
#include "uclistener.h"
#include <ucstring.h>
CUCOREListener::CUCOREListener()
{
	m_poPacketParser	= NULL;
	m_poSessionFactory	= NULL;
	m_dwRecvBufSize		= DEFAULT_RECVBUF_SIZE;
	m_dwSendBufSize		= DEFAULT_SENDBUF_SIZE;
	m_poCpListener		= NULL;
	m_bStart			= false;
    m_bNodelay   = FALSE;
}

CUCOREListener::~CUCOREListener()
{

}

void CUCOREListener::SetPacketParser(ISDPacketParser* poPacketParser)
{
	m_poPacketParser = poPacketParser;
}

void CUCOREListener::SetSessionFactory(ISDSessionFactory* poSessionFactory)
{
	m_poSessionFactory = poSessionFactory;
}

void CUCOREListener::SetBufferSize(UINT32 dwRecvBufSize, UINT32 dwSendBufSize)
{
	m_dwRecvBufSize	= dwRecvBufSize;
	m_dwSendBufSize	= dwSendBufSize;
}

void CUCOREListener::SetOpt(UINT32 dwType, void* pOpt)
{
    switch(dwType)
    {
    case TCP_NO_DELAY:
        {
            STcpOptSockNoDelay* pstTcpOptSockNoDelay = (STcpOptSockNoDelay*)pOpt;
            if (pstTcpOptSockNoDelay)
            {
                m_bNodelay = pstTcpOptSockNoDelay->bNodelay;
            }
        }
        break;
    }

}

bool CUCOREListener::Start(const char* pszIP, UINT16 wPort, bool bReUseAddr)
{
	if(m_bStart)
	{
		return true;
	}

    WARN(_SDT("CUCOREListener::Start Listen %s:%d, ReuseAddr %d"), _SDTA2T(pszIP), wPort, bReUseAddr);

	if(NULL == m_poSessionFactory)
	{
		return false;
	}

    m_poCpListener = new CCpListener;
	if(NULL == m_poCpListener)
	{
		CRITICAL(_SDT("CUCOREListener::Start, new CCpListener failed"));
		return false;
	}
    m_poCpListener->SetNoDelay(m_bNodelay);

	m_poCpListener->SetID(0);
	m_poCpListener->SetBufferSize(m_dwRecvBufSize, m_dwSendBufSize);
	m_poCpListener->SetPacketParser(m_poPacketParser);
	m_poCpListener->SetSessionFactory(m_poSessionFactory);
	if(false == m_poCpListener->Start(pszIP, wPort, bReUseAddr))
	{
		return false;
	}

	m_bStart = true;

	return true;
}

bool CUCOREListener::Stop(void)
{
	if(false == m_bStart)
	{
		return true;
	}

    WARN(_SDT("CUCOREListener::Stop"));

	if(m_poCpListener != NULL)
	{
		m_poCpListener->Stop();
		//Sleep(500);
		delete m_poCpListener;
		m_poCpListener = NULL;
	}

	m_bStart = false;
	return true;
}

void CUCOREListener::Release(void)
{
	delete this;
}


#include "StdAfx.h"
#include "PGHttpFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPGHttpFile::CPGHttpFile() : m_mapHeader(), m_pFile(NULL), m_pHttpConnection(NULL)
{
}

CPGHttpFile::~CPGHttpFile()
{
	Disconnect();
}

HRESULT CPGHttpFile::Connect(LPCWSTR szServer, LPCWSTR szObjName, INTERNET_PORT nPort, int nVerb, DWORD dwFlag)
{
	Disconnect();

	try
	{
		m_pHttpConnection = m_session.GetHttpConnection(szServer, INTERNET_FLAG_KEEP_CONNECTION, nPort);

		m_pFile = m_pHttpConnection->OpenRequest(nVerb, szObjName, NULL, 1UL, NULL, NULL, dwFlag);
	}
	catch (CInternetException* e)
	{
		TCHAR whatMsg[128] = {0, };
		e->GetErrorMessage(whatMsg, 128);
		e->Delete();
		return E_UNEXPECTED;
	}
	catch (CException* e)
	{
		e->Delete();
		return E_UNEXPECTED;
	}
	return S_OK;
}

HRESULT CPGHttpFile::Connect(LPCSTR szServer, LPCSTR szObjName, INTERNET_PORT nPort, int nVerb, DWORD dwFlag)
{
	int nLenServer = ::MultiByteToWideChar(CP_ACP, 0, szServer, strlen(szServer), NULL, 0);
	int nLenObjName = ::MultiByteToWideChar(CP_ACP, 0, szObjName, strlen(szObjName), NULL, 0);

	wchar_t* pData = new wchar_t[nLenServer + 1 + nLenObjName + 1];
	::MultiByteToWideChar(CP_ACP, 0, szServer, strlen(szServer), pData, nLenServer);
	pData[nLenServer] = NULL;
	::MultiByteToWideChar(CP_ACP, 0, szObjName, strlen(szObjName), &pData[nLenServer + 1], nLenObjName);
	pData[nLenServer + nLenObjName + 1] = NULL;

	HRESULT hr = Connect(pData, &pData[nLenServer + 1], nPort, nVerb, dwFlag);
	delete[] pData;

	return hr;
}

HRESULT CPGHttpFile::Disconnect()
{
	if (NULL != m_pFile)
	{
		m_pFile->Close();
		delete m_pFile;
		m_pFile = NULL;
	}
	if (NULL != m_pHttpConnection)
	{
		m_pHttpConnection->Close();
		delete m_pHttpConnection;
		m_pHttpConnection = NULL;
	}
	//m_session.Close();

	PGHeaderMap mapTemp;
	m_mapHeader.swap(mapTemp);	// Init header map...

	return S_OK;
}

HRESULT CPGHttpFile::AddHeader(LPCWSTR szHeader, LPCWSTR szValue)
{
	m_mapHeader.insert(PGHeaderMap::value_type(std::wstring(szHeader), std::wstring(szValue)));

	return S_OK;
}

HRESULT CPGHttpFile::AddHeader(LPCSTR szHeader, LPCSTR szValue)
{
	int nLenHeader = ::MultiByteToWideChar(CP_ACP, 0, szHeader, strlen(szHeader), NULL, 0);
	int nLenValue = ::MultiByteToWideChar(CP_ACP, 0, szValue, strlen(szValue), NULL, 0);

	wchar_t* pData = new wchar_t[nLenHeader + 1 + nLenValue + 1];
	::MultiByteToWideChar(CP_ACP, 0, szHeader, strlen(szHeader), pData, nLenHeader);
	pData[nLenHeader] = NULL;
	::MultiByteToWideChar(CP_ACP, 0, szValue, strlen(szValue), &pData[nLenHeader + 1], nLenValue);
	pData[nLenHeader + nLenValue + 1] = NULL;

	HRESULT hr = AddHeader(pData, &pData[nLenHeader + 1]);
	delete[] pData;

	return hr;
}

HRESULT CPGHttpFile::AddBase64Authentication(LPCWSTR szIDPass)
{
	int nLenIDPass = ::WideCharToMultiByte(CP_UTF8, 0, szIDPass, wcslen(szIDPass), NULL, 0, NULL, NULL);

	char* pData = new char[nLenIDPass + 1];
	::WideCharToMultiByte(CP_UTF8, 0, szIDPass, wcslen(szIDPass), pData, nLenIDPass, NULL, NULL);
	pData[nLenIDPass] = NULL;

	int nEncodeSize = Base64EncodeGetRequiredLength(nLenIDPass);
	char* szEncode = new char[nEncodeSize];
	memset(szEncode, 0x00, nEncodeSize);

	BOOL bRtn = Base64Encode((BYTE*)pData, nLenIDPass, szEncode, &nEncodeSize);
	if (FALSE == bRtn)
	{
		delete[] pData;
		delete[] szEncode;

		return E_FAIL;
	}
		
	std::string szValue = "Basic ";
	szValue += szEncode;
	HRESULT hr = AddHeader("Authorization", szValue.c_str());
	delete[] pData;
	delete[] szEncode;

	return (TRUE == SUCCEEDED(hr)) ? S_OK : E_FAIL;
}

HRESULT CPGHttpFile::AddBase64Authentication(LPCSTR szIDPass)
{
	int nLenIDPass = ::MultiByteToWideChar(CP_ACP, 0, szIDPass, strlen(szIDPass), NULL, 0);

	wchar_t* pData = new wchar_t[nLenIDPass + 1];
	::MultiByteToWideChar(CP_ACP, 0, szIDPass, strlen(szIDPass), pData, nLenIDPass);
	pData[nLenIDPass] = NULL;

	HRESULT hr = AddBase64Authentication(pData);

	delete[] pData;

	return hr;
}

HRESULT CPGHttpFile::Write(LPVOID lpData, DWORD dwCount, DWORD& dwStatus, __int64& nReadableSize, std::wstring& szResult)
{
	if (NULL == m_pFile || NULL == m_pHttpConnection)
		return E_FAIL;

	std::wstring szHeader;
	szHeader.reserve(4096);

	PGHeaderMap::const_iterator it = m_mapHeader.begin();
	for ( ; m_mapHeader.end() != it ; it++)
	{
		szHeader = it->first + L": " + it->second;
		m_pFile->AddRequestHeaders(szHeader.c_str());
	}

	try
	{
		if (FALSE == m_pFile->SendRequest(NULL, 0, lpData, dwCount))
		{
			return E_FAIL;
		}
	}
	catch (CInternetException* /*e*/)
	{
		return E_FAIL;
	}
	catch (CException* /*e*/)
	{
		return E_FAIL;
	}

	m_pFile->QueryInfoStatusCode(dwStatus);

	TCHAR szSize[64];
	DWORD dwSize = sizeof(szSize);
	m_pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, szSize, &dwSize);
	nReadableSize = _ttoi64(szSize);

	dwSize = sizeof(szSize);
	m_pFile->QueryInfo(HTTP_QUERY_STATUS_TEXT, szSize, &dwSize);
	szResult = CT2CW(szSize);

	CString szTemps;
	m_pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, szTemps);

		
	return S_OK;
}

HRESULT CPGHttpFile::Read(LPCWSTR szFileName, BOOL bOverWrite/* = TRUE*/)
{
	if (NULL == m_pFile || NULL == m_pHttpConnection)
		return E_FAIL;

	if (NULL == szFileName)
		return E_INVALIDARG;

	DWORD ulTotalSize = 0;
	DWORD ulCurrentSize = 0;

	m_pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, ulTotalSize);

	HANDLE hFile = ::CreateFileW(szFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, (TRUE == bOverWrite) ? CREATE_ALWAYS : CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_ACCESSDENIED;

	char* buffer = new char[1024];
	while (ulTotalSize > ulCurrentSize)
	{
		DWORD dwNumberOfBytesWritten = 0;
		memset(buffer, 0x00, 1024);
		UINT nRead = (ulTotalSize - ulCurrentSize) < 1024 ? ulTotalSize - ulCurrentSize : 1024;
		m_pFile->Read(buffer, nRead);
		::WriteFile(hFile, buffer, nRead, &dwNumberOfBytesWritten, NULL);
		ulCurrentSize += nRead;
	}

	delete[] buffer;
	::CloseHandle(hFile);

	return S_OK;
}

HRESULT CPGHttpFile::Read(LPCSTR szFileName, BOOL bOverWrite/* = TRUE*/)
{
	int nLenPath = ::MultiByteToWideChar(CP_ACP, 0, szFileName, strlen(szFileName), NULL, 0);

	wchar_t* pData = new wchar_t[nLenPath + 1];
	::MultiByteToWideChar(CP_ACP, 0, szFileName, strlen(szFileName), pData, nLenPath);
	pData[nLenPath] = NULL;

	HRESULT hr = Read(pData, bOverWrite);

	delete[] pData;

	return hr;
}

HRESULT CPGHttpFile::Read(LPVOID lpData, DWORD& dwBufferLen, DWORD dwOffset, DWORD dwSize)
{
	if (NULL == lpData || dwSize > dwBufferLen)
		return E_INVALIDARG;

	if (dwBufferLen <= dwSize)
		return E_INVALIDARG;

	return S_OK;
}

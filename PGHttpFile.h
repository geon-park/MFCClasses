#pragma once

#include <Windows.h>
#include <unordered_map>
#include <afxinet.h>
#include <atlenc.h>

typedef std::unordered_map<std::wstring, std::wstring> PGHeaderMap;

class CPGHttpFile
{
public:
	CPGHttpFile();
	~CPGHttpFile();

public:
	//HRESULT Connect(LPWSTR const szURI, );
	//HRESULT Connect(LPSTR const szURI, );

	HRESULT Connect(LPCWSTR szServer, LPCWSTR szObjName, INTERNET_PORT nPort, int nVerb, DWORD dwFlag);
	HRESULT Connect(LPCSTR szServer, LPCSTR szObjName, INTERNET_PORT nPort, int nVerb, DWORD dwFlag);
	HRESULT Disconnect();

	HRESULT AddHeader(LPCWSTR szHeader, LPCWSTR szValue);
	HRESULT AddHeader(LPCSTR szHeader, LPCSTR szValue);
	HRESULT AddBase64Authentication(LPCWSTR szIDPass);	// szIDPass => ID:Password
	HRESULT AddBase64Authentication(LPCSTR szIDPass);	// szIDPass => ID:Password

	HRESULT Write(LPVOID lpData, DWORD dwCount, DWORD& dwStatus, __int64& nReadableSize, std::wstring& szResult);
	HRESULT Read(LPCWSTR szFileName, BOOL bOverWrite = TRUE);
	HRESULT Read(LPCSTR szFileName, BOOL bOverWrite = TRUE);
	HRESULT Read(LPVOID lpData, DWORD& dwBufferLen, DWORD dwOffset, DWORD dwSize);

private:
	PGHeaderMap m_mapHeader;

	CInternetSession m_session;
	CHttpFile* m_pFile;
	CHttpConnection* m_pHttpConnection;
};
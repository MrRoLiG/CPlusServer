#include "stdafx.h"

CMySQLManager::CMySQLManager()
{
	_connect();
}
CMySQLManager::~CMySQLManager()
{
	_disConnect();
}

void CMySQLManager::_connect()
{
	//��ʼ��OLE/COM�⻷��
	::CoInitialize(NULL);
	HRESULT hr;

	try{
		//����Connection����
		hr = m_pConnection.CreateInstance("ADODB.Connection");
		if (SUCCEEDED(hr))
		{
			m_pConnection->ConnectionTimeout = 600;
			m_pConnection->CommandTimeout = 120;
			//m_pConnection->Open("DSN=MySQL;Server=127.0.0.1;Database=test", "root", "root", adModeUnknown);
			m_pConnection->Open("DRIVER={MySQL ODBC 8.0 Unicode Driver};Server=127.0.0.1;Database=test", "root", "root", adModeUnknown);
		}
	}
	catch (_com_error e)
	{
		printf(e.Description());
	}
}

void CMySQLManager::_disConnect()
{
	//�ر��������¼��
	if (NULL != m_pRecordset)
	{
		m_pRecordset->Close();
	}
	m_pConnection->Close();

	//�ͷ�OLE/COM����
	::CoUninitialize();
}

_ConnectionPtr& CMySQLManager::GetConnection()
{
	return m_pConnection;
}
_RecordsetPtr& CMySQLManager::GetRecordset()
{
	return m_pRecordset;
}
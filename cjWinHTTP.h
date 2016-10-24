#pragma once

#include <winhttp.h>

namespace cj {

	class WinHttp {
	protected:
	public:
		HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;

		WinHttp();
		~WinHttp();

		virtual bool open();
		virtual bool connect(String serverName, int serverPort);
		virtual bool openRequest(String method, String url, bool ssl);
		virtual bool setOptions(ulong options, ulong flags);
		virtual bool sendRequest();
		virtual bool receiveResponse();
		virtual int queryDataAvailable();
		virtual int readData(Memory &memory, int size = 0);
	};

}


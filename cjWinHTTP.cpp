#include "cj.h"
#include "cjWinHttp.h"

namespace cj {
	WinHttp::WinHttp() {
	}

	WinHttp::~WinHttp() {
		// Close any open handles.
		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);
	}

	bool WinHttp::open() {
		hSession = WinHttpOpen(L"WinHTTP Example/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
			WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

		return hSession != nullptr;
	}

	bool WinHttp::connect(String serverName, int serverPort) {
		wstring ws = serverName.to_wstring();
		LPCWSTR s = ws.c_str();
		
		hConnect = WinHttpConnect(hSession, s, serverPort, 0);
		return hConnect != nullptr;
	}

	bool WinHttp::openRequest(String method, String url, bool ssl) {
		if (ssl) {
			hRequest = WinHttpOpenRequest(hConnect, method.to_wstring().c_str(), url.to_wstring().c_str(),
				NULL, WINHTTP_NO_REFERER,
				WINHTTP_DEFAULT_ACCEPT_TYPES,
				WINHTTP_FLAG_SECURE);
		}
		else {
			hRequest = WinHttpOpenRequest(hConnect, method.to_wstring().c_str(), url.to_wstring().c_str(),
				NULL, WINHTTP_NO_REFERER,
				WINHTTP_DEFAULT_ACCEPT_TYPES,
				0);
		}
		return hRequest != nullptr;
	}

	bool WinHttp::setOptions(ulong options, ulong flags) {
		return WinHttpSetOption(hRequest, options, &flags, sizeof(ulong));
	}

	bool WinHttp::sendRequest() {
		return WinHttpSendRequest(hRequest, /*L"content-type:application/application/octet-stream"*/WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	}

	bool WinHttp::receiveResponse() {
		return WinHttpReceiveResponse(hRequest, NULL);
	}

	int WinHttp::queryDataAvailable() {
		ulong dwSize = 0;
		bool flag = WinHttpQueryDataAvailable(hRequest, &dwSize);
		if (!flag) return -1;
		return dwSize;
	}

	int WinHttp::readData(Memory &memory, int size) {
		if (size == 0) {
			int result = queryDataAvailable();
			if (result == -1) return -1;
			if (result == 0) return 0;
			size = result;
		}

		// Allocate space for the buffer.
		LPSTR pszOutBuffer = new char[size];
		if (!pszOutBuffer)
		{
			printf("Out of memory\n");
			return -2;
		}
		ulong dwDownloaded;
		bool flag = WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, size, &dwDownloaded);
		if (!dwDownloaded) return -3;

		memory.write(pszOutBuffer, dwDownloaded);
		// Free the memory allocated to the buffer.
		delete[] pszOutBuffer;

		return dwDownloaded;
	}
}
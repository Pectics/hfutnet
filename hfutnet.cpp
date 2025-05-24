#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <string>

#pragma comment(lib, "wininet.lib")

std::string gb2312_to_utf8(const std::string& str_gb2312) {
    int len_wide = MultiByteToWideChar(936, 0, str_gb2312.c_str(), -1, NULL, 0); // 936 = GB2312
    std::wstring wide(len_wide, 0);
    MultiByteToWideChar(936, 0, str_gb2312.c_str(), -1, &wide[0], len_wide);

    int len_utf8 = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, NULL, 0, NULL, NULL);
    std::string str_utf8(len_utf8, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, &str_utf8[0], len_utf8, NULL, NULL);

    return str_utf8;
}

void print_usage() {
    std::cout << "Usage: hfutnet.exe -s <sid> -p <password>\n";
}

bool parse_args(int argc, char* argv[], std::string& sid, std::string& password) {
    for (int i = 1; i < argc - 1; ++i) {
        std::string arg = argv[i];
        if ((arg == "-s" || arg == "--sid") && i + 1 < argc) {
            sid = argv[++i];
        } else if ((arg == "-p" || arg == "--password") && i + 1 < argc) {
            password = argv[++i];
        }
    }
    return !(sid.empty() || password.empty());
}

int main(int argc, char* argv[]) {
    std::string sid, password;

    if (!parse_args(argc, argv, sid, password)) {
        print_usage();
        return 1;
    }

    HINTERNET hInternet = InternetOpen("HFUTLogin", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        std::cerr << "InternetOpen failed\n";
        return 1;
    }

    HINTERNET hConnect = InternetConnect(
        hInternet, "172.16.200.11", INTERNET_DEFAULT_HTTP_PORT,
        NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);

    if (!hConnect) {
        std::cerr << "InternetConnect failed\n";
        InternetCloseHandle(hInternet);
        return 1;
    }

    HINTERNET hRequest = HttpOpenRequest(
        hConnect, "POST", "/", NULL, NULL, NULL,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);

    if (!hRequest) {
        std::cerr << "HttpOpenRequest failed\n";
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }

    std::string postData = "DDDDD=" + sid + "&upass=" + password + "&0MKKey=123";
    const char* headers = "Content-Type: application/x-www-form-urlencoded";

    BOOL bSend = HttpSendRequestA(
        hRequest, headers, strlen(headers),
        (LPVOID)postData.c_str(), (DWORD)postData.length());

    if (!bSend) {
        std::cerr << "HttpSendRequest failed\n";
    } else {
        char buffer[1025];
        DWORD bytesRead = 0;
        std::string response;
        while (InternetReadFile(hRequest, buffer, 1024, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            response += buffer;
        }

        response = gb2312_to_utf8(response);
        if (response.find("您已经成功登录") != std::string::npos) {
            std::cout << "Successfully logged in. Check browser or status page.\n";
        } else {
            std::cout << "Login response unclear. Raw response length: " << response.length() << '\n';
        }
    }

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return 0;
}

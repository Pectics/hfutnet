#include <windows.h>
#include <wininet.h>
#include <wincrypt.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cctype>

#include "hfutnet/consts.h"
#include "hfutnet/utils.hpp"

#pragma comment(lib, "wininet.lib")

enum Operation {
    LOGIN_OPERATION = 0,
    LOGOUT_OPERATION = 1,
};

inline bool debug_mode = false;
inline std::ostringstream headers;

void print_usage() {
    std::cout <<
R"(Usage:
  hfutnet.exe <sid> <password> [--login | -i]
  hfutnet.exe [<sid>] [<password>] (--logout | -o)

Description:
  hfutnet is a command-line tool for logging in to or logging out of the
  HFUT campus network.

Arguments:
  <sid>         Student ID (required for login)
  <password>    Network password (required for login)

Options:
  --login, -i   Log in to the campus network (default action)
  --logout,-o   Log out from the campus network

Behavior:
  - If no option is specified, the program defaults to --login (-i).
  - When using --login (-i):
      <sid> and <password> are required.
  - When using --logout (-o):
      <sid> and <password> are optional.

Examples:
  hfutnet.exe 2024213492 mypassword
  hfutnet.exe 2024213492 mypassword --login
  hfutnet.exe --logout
  hfutnet.exe 2024213492 --logout
)";
}

void login(const HINTERNET& internet,
           const HINTERNET& connect,
           const std::string& sid,
           const std::string& password) {

    HINTERNET hRequest = HttpOpenRequestA(
        connect, "POST", "/0.htm", NULL, NULL, NULL,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    if (!hRequest) throw std::runtime_error("HttpOpenRequest failed");

    std::string response;
    try {
        // Construct form data
        // Dr.COM uses: DDDDD=<sid>&upass=md5(pid+<password>+calg)&R1=0&R2=1&para=00&0MKKey=123456&v6ip=
        std::string upass_md5 = md5_hex(COM_PID + password + COM_CALG);
        std::ostringstream form;
        form << "DDDDD=" << url_encode(sid)
            << "&upass=" << upass_md5 << COM_CALG << COM_PID
            << "&R1=0&R2=1&para=00&0MKKey=" << COM_0MKKEY
            << "&v6ip=";
        std::string body = form.str();

        BOOL bSend = HttpSendRequestA(
            hRequest, headers.str().c_str(), (DWORD) headers.str().length(),
            (LPVOID) body.c_str(), (DWORD) body.size());
        if (!bSend) throw std::runtime_error("HttpSendRequest failed");
        
        char buffer[1025];
        DWORD bytesRead = 0;
        while (InternetReadFile(hRequest, buffer, 1024, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            response += buffer;
        }
        response = gb2312_to_utf8(response);

        // Login success check
        if (std::regex_search(response, REGEX_LOGIN_SUCCESS)) {
            std::cout << "登录成功！\n";
            InternetCloseHandle(hRequest);
            return;
        }

        // Login failure, parse message code
        std::smatch match;

        // No message code found
        if (!std::regex_search(response, match, REGEX_MSG_CODE))
            throw std::runtime_error("无法解析响应消息");

        // Message code found, convert to integer
        int msg_code;
        try {
            msg_code = std::stoi(match[1].str());
        } catch (...) {
            throw std::runtime_error("无法解析消息代码");
        }

        switch (msg_code) {
            case 0:
            case 1: {
                std::smatch info_match;
                if (!std::regex_search(response, info_match, REGEX_MSG_INFO))
                throw std::runtime_error("未知错误");
                std::string info = info_match[1].str();
                if (info == "error0")           throw std::runtime_error("当前IP地址已被禁止使用Web方式登录");
                if (info == "error1")           throw std::runtime_error("当前账号已被禁止使用Web方式登录");
                if (info == "error2")           throw std::runtime_error("当前账号不允许修改密码"); // what?!
                if (info == "userid error1")    throw std::runtime_error("账号不存在");
                if (info == "userid error2")    throw std::runtime_error("密码错误");
                if (info == "userid error3")    throw std::runtime_error("密码错误");
                throw std::runtime_error("登录失败：" + info);
            }
            case 2:     throw std::runtime_error("当前账号正在使用中");
            case 3:     throw std::runtime_error("当前账号只能在指定地址使用");
            case 4:     throw std::runtime_error("当前账号费用超支或时长、流量超过限制");
            case 5:     throw std::runtime_error("当前账号已暂停使用");
            case 6:     throw std::runtime_error("系统缓存已满，请稍后再试");
            case 7:     throw std::runtime_error("UT+UF+UM（疑似用量统计）"); // what?!
            case 8:     throw std::runtime_error("当前账号正在使用，无法修改"); // what?!
            case 9:     throw std::runtime_error("密码确认不匹配，无法修改"); // what?!
            case 10:    throw std::runtime_error("密码修改成功"); // what?!
            case 11:    throw std::runtime_error("当前账号只能在指定地址使用");
            case 14:    throw std::runtime_error("账号注销成功");
            case 15:    throw std::runtime_error("登录成功（？何意味）"); // what?!
        }
    } catch (std::exception& e) {
        std::cerr << "登录失败：" << e.what() << "。\n";
        if (debug_mode) 
            std::cerr << "Response was:\n" << response << "\n";
        InternetCloseHandle(hRequest);
        return;
    }
    
    InternetCloseHandle(hRequest);
    return;
}

void logout() {
    // Implementation of logout function (if needed separately)
}

int main(int argc, char* argv[]) {

    SetConsoleOutputCP(CP_UTF8);

    std::string sid, password;
    auto op = LOGIN_OPERATION;
    bool op_set = false;
    auto set_op = [&](Operation _op) -> bool {
        if (op_set && op != _op) {
            std::cerr << "Conflicting operations specified.\n";
            print_usage();
            return false;
        }
        op = _op;
        op_set = true;
        return true;
    };

    // Parse command-line arguments
    bool pos_only = false;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        /** Parse flags **/
        if (!pos_only) {
            if (arg == "--")                        { pos_only = true; continue; }
            if (arg == "--help" || arg == "-h")     { print_usage(); return 0; }
            if (arg == "--version" || arg == "-v")  { std::cout << "1.0\n"; return 0; }
            if (arg == "--debug" || arg == "-d")    { debug_mode = true; continue; }
            if (arg == "--logout" || arg == "-o")   { if (!set_op(LOGOUT_OPERATION)) return 1; continue; }
            if (arg == "--login" || arg == "-i")    { if (!set_op(LOGIN_OPERATION))  return 1; continue; }
            if (!arg.empty() && arg[0] == '-') {
                std::cerr << "Unknown flag: " << arg << "\n";
                print_usage();
                return 1;
            }
        }
        /** Parse positional **/
        /**/ if (sid.empty())   sid = arg;
        else if (password.empty())  password = arg;
        else {
            std::cerr << "Too many arguments: " << arg << "\n";
            print_usage();
            return 1;
        }
    }

    // Validate required arguments
    if (op == LOGIN_OPERATION && (sid.empty() || password.empty())) {
        std::cerr << "SID and password are required for login operation.\n";
        print_usage();
        return 1;
    }

    HINTERNET hInternet = InternetOpenA(
        "HFUT Campus Network", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        std::cerr << "InternetOpen failed\n";
        return 2;
    }

    HINTERNET hConnect = InternetConnectA(
        hInternet, "172.16.200.11", INTERNET_DEFAULT_HTTP_PORT,
        NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        std::cerr << "InternetConnect failed\n";
        InternetCloseHandle(hInternet);
        return 2;
    }

    headers << "User-Agent: "                   << USER_AGENT                   << "\r\n"
            << "Accept: "                       << ACCEPT                       << "\r\n"
            << "Accept-Language: "              << ACCEPT_LANGUAGE              << "\r\n"
            << "Accept-Encoding: "              << ACCEPT_ENCODING              << "\r\n"
            << "Cache-Control: "                << CACHE_CONTROL                << "\r\n"
            << "Connection: "                   << CONNECTION                   << "\r\n"
            << "Content-Type: "                 << CONTENT_TYPE                 << "\r\n"
            << "Upgrade-Insecure-Requests: "    << UPGRADE_INSECURE_REQUESTS    << "\r\n";

    // Perform the operation
    switch (op) {
        case LOGIN_OPERATION:
            login(hInternet, hConnect, sid, password);
            break;
        case LOGOUT_OPERATION:
            logout();
            break;
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return 0;
}

#pragma once

#include <regex>
#include <string>

// Global HTTP Headers Constants
inline const std::string ACCEPT_ENCODING =              "gzip, deflate";
inline const std::string ACCEPT_LANGUAGE =              "zh-CN,zh;q=0.9,en-US;q=0.8,en;q=0.7,en-GB;q=0.6";
inline const std::string CACHE_CONTROL =                "max-age=0";
inline const std::string CONNECTION =                   "keep-alive";
inline const std::string CONTENT_TYPE =                 "application/x-www-form-urlencoded";
inline const std::string UPGRADE_INSECURE_REQUESTS =    "1";
inline const std::string USER_AGENT =
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
    "AppleWebKit/537.36 (KHTML, like Gecko) "
    "Chrome/91.0.4472.124 "
    "Safari/537.36";
inline const std::string ACCEPT =
    "text/html,application/xhtml+xml,application/xml;q=0.9,"
    "image/avif,image/webp,image/apng,*/*;q=0.8,"
    "application/signed-exchange;v=b3;q=0.7";

// Dr.COM form constants
inline const std::string COM_PID = "2";
inline const std::string COM_CALG = "12345678";
inline const std::string COM_0MKKEY = "123456";

// Available hosts
// TODO: Reserve for future expansion
inline const std::string HOSTS[] = {
    "172.16.200.11",
    "172.16.200.12",
};

// Regular expressions
inline const std::regex REGEX_LOGIN_SUCCESS(
    R"(<title>[ \r\n\t]*登录成功[ \r\n\t]*</title>)", std::regex::icase);
inline const std::regex REGEX_MSG_CODE(R"(Msg=(\d+);)");
inline const std::regex REGEX_MSG_INFO(R"(msga='([a-zA-Z0-9 ]+)';)");
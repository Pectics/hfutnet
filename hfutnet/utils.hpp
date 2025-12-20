#pragma once

#include <windows.h>
#include <wincrypt.h>
#include <string>
#include <sstream>
#include <iomanip>

#pragma comment(lib, "crypt32.lib")

std::string gb2312_to_utf8(const std::string& str_gb2312) {

    int len_wide = MultiByteToWideChar(936, 0, str_gb2312.c_str(), -1, NULL, 0); // 936 = GB2312
    std::wstring wide(len_wide, 0);
    MultiByteToWideChar(936, 0, str_gb2312.c_str(), -1, &wide[0], len_wide);

    int len_utf8 = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, NULL, 0, NULL, NULL);
    std::string str_utf8(len_utf8, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, &str_utf8[0], len_utf8, NULL, NULL);

    return str_utf8;
}

std::string url_encode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::uppercase << std::hex;

    for (unsigned char c : value) {
        if (std::isalnum(c) || strchr("-_.*", c) != nullptr) escaped << static_cast<char>(c);
        else if (c == ' ') escaped << '+';
        else escaped << '%' << std::setw(2) << static_cast<int>(c);
    }
    return escaped.str();
}

std::string md5_hex(const std::string& data) {

    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE rgbHash[16];
    DWORD cbHash = sizeof(rgbHash);

    if (!CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) return "";
    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        return "";
    }
    if (!CryptHashData(hHash, reinterpret_cast<const BYTE*>(data.data()), (DWORD)data.size(), 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return "";
    }
    if (!CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return "";
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

    std::ostringstream oss;
    oss << std::hex << std::nouppercase;
    for (DWORD i = 0; i < cbHash; ++i)
        oss << std::setw(2) << std::setfill('0') << (int)rgbHash[i];
    return oss.str();
}
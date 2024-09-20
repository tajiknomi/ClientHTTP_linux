// Copyright (c) Nouman Tajik [github.com/tajiknomi]
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE. 


#include "systemInformation.h"
#include <unistd.h>
#include <codecvt>
#include <random>
#include <ctime>
#include <sys/types.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <cstring>
#include <locale>

std::string SysInformation::generateRandomAlphanumeric(const int &length, const long long &seed) {
    std::string alphanumeric = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<int> dist(0, alphanumeric.length() - 1);

    std::string result;
    for (int i = 0; i < length; ++i) {
        result += alphanumeric[dist(rng)];
    }
    return result;
}

std::wstring SysInformation::s2ws(const std::string& str) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

std::wstring SysInformation::getComputerName() {
    char computerName[250] = {};
    if (gethostname(computerName, sizeof(computerName)) != 0) {
        perror("gethostname");
        return L""; // Handle the error accordingly, returning an empty wstring in this case
    }
    return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(computerName);
}

std::wstring SysInformation::getUserName()
{
  uid_t uid = geteuid();
  struct passwd *pw = getpwuid(uid);
  if (pw)
  {
    return s2ws(pw->pw_name);
  }
  return {};
}

std::vector<std::wstring> SysInformation::getSysInfo() {
    std::vector<std::wstring> data_vec;
    std::wstring computerName = getComputerName(); // Use a wide string version of getComputerName

    // Generate the seed from time and computer name
    std::time_t currentTime = std::time(nullptr);

    std::wstring seedString = std::to_wstring(currentTime) + computerName;
    std::hash<std::wstring> seedHash;
    long long seed = static_cast<long long>(seedHash(seedString));

    // Generate a random alphanumeric number
    std::wstring randomAlphanumeric = s2ws(generateRandomAlphanumeric(RANDOM_NUMBER_LENGTH, seed)); // Use a wide string version of generateRandomAlphanumeric

    data_vec.push_back(L"id");
    data_vec.push_back(randomAlphanumeric);

    std::wstring username = getUserName(); // Use a wide string version of getUserName

    struct utsname sysInfo;
    if (uname(&sysInfo) == 0);
    else { // Couldn't retrieve sysInfo
        memset(&sysInfo, 0x00, sizeof(struct utsname));
    }

    data_vec.push_back(L"username");
    data_vec.push_back(username);
    data_vec.push_back(L"computerName");
    data_vec.push_back(computerName);
    data_vec.push_back(L"OSname");
    data_vec.push_back(s2ws(sysInfo.sysname));
    data_vec.push_back(L"OSversion");
    data_vec.push_back(s2ws(sysInfo.version).substr(0, 19));
    return data_vec;
}
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



#pragma once

#include "utilities.h"
#include <string>
#include <vector>

#if __has_include(<filesystem>)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #error "Neither <filesystem> nor <experimental/filesystem> are available."
#endif

std::wstring createHeartbeatRequest(const std::wstring &sysInfoInJson);

bool isValidPort(const std::string& portNum);

bool hasWritePermissionForDirectory(const std::wstring &dirPath);

std::wstring changeDir(const std::wstring& newPath, std::error_code& ec);

std::wstring ExtractLastDirectoryName(const std::wstring& path);

bool isExecutable(const std::wstring& path);

std::size_t calculateDirectorySize(const std::string& path);

std::string extractBase64Data(const std::wstring &buff);

std::wstring ReplaceTildeWithPath(const std::wstring& filePath);

bool isDataServerAvailable(const std::string& url);


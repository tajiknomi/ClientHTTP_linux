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


#include "sharedResourceManager.h"

void SharedResourceManager::pushResponse(const std::wstring &response) {
	std::lock_guard<std::mutex> lock(responseQueueMutex);
	if (! (response.empty()) ) {
		responseQueue.push(response);
	}
}

std::wstring SharedResourceManager::popResponse(void) {
	std::lock_guard<std::mutex> lock(responseQueueMutex);
	std::wstring response;
	if(! (responseQueue.empty()) ){
		response = responseQueue.front();
		responseQueue.pop();
	}
	return response;
}

void SharedResourceManager::pushJob(const std::wstring &job) {
	std::lock_guard<std::mutex> lock(jobQueueMutex);
	if (! (job.empty()) ) {
		jobQueue.push(job);
	}
}

std::wstring SharedResourceManager::popJob(void) {
	std::lock_guard<std::mutex> lock(jobQueueMutex);
	std::wstring job;
	if ( !(jobQueue.empty()) ) {
		job = jobQueue.front();
		jobQueue.pop();
	}
	return job;
}

void SharedResourceManager::setSysInfoInJson(const std::wstring &sysInfo) {
	std::lock_guard<std::mutex> lock(jsonSysInfoMutex);
	jsonSysInfo = sysInfo;
}

std::wstring SharedResourceManager::getSysInfoInJson(void) {
	std::lock_guard<std::mutex> lock(jsonSysInfoMutex);
	return jsonSysInfo;
}

bool SharedResourceManager::isResponseAvailable(void) {	
	std::lock_guard<std::mutex> lock(responseQueueMutex);
	return !responseQueue.empty();
}
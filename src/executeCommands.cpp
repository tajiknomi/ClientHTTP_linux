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


#include "executeCommands.h"
#include <unistd.h>
#include <iconv.h>
#include "stringUtil.h"
#include <iostream>
#include <sys/wait.h>
#include <sstream>
#include <cstring>

std::wstring executeCommand::operator()(const std::wstring& shellType, const std::wstring& command, const std::wstring& args) {

    // Create a pipe for IPC
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return L"";
    }
    // Fork the process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return L"";
    } 
    else if (pid == 0) {    // Child process
        // Close the read end of the pipe
        close(pipefd[0]);

        // Redirect stdout/stderr to the write end of the pipe
        if(dup2(pipefd[1], STDOUT_FILENO) == -1){
            return L"Redirection of stdout to pipe for child process failed";
        }
        if(dup2(pipefd[1], STDERR_FILENO) == -1){
            return L"Redirection of stderr to pipe for child process failed";
        }
        std::wstring fullCommand = command + L" " + args;
        int retValue;

        // Extract the parent environment variables
        extern char** environ;
        std::vector<char*> envVariables;
        for (char** env = environ; *env != nullptr; ++env) {
            envVariables.push_back(*env);
        }
        envVariables.push_back(nullptr); // Null-terminate the array

        if(shellType.empty()){      // This branch is used to execute a program 
            const char** execArgs = nullptr;
            retValue = execve(StringUtils::wstring_to_utf8(command).c_str(), const_cast<char* const*>(execArgs), envVariables.data());          
        }
        else{   // Execute command on the supplied shell e.g. (/bin/sh)
            // Execute the command
            std::vector<char*> shell = {strdup(StringUtils::wstring_to_utf8(shellType).c_str()), strdup("-c"), strdup(StringUtils::wstring_to_utf8(fullCommand).c_str()), nullptr};
            retValue = execve(StringUtils::wstring_to_utf8(shellType).c_str(), shell.data(), envVariables.data());   
        }
        if(retValue == -1){     // check for execve(..) error
            char buff[50];
            std::wcout << L"execute: " + fullCommand + L" returns : " + StringUtils::s2ws(strerror_r(errno, buff, sizeof(buff)));
        }       
        exit(retValue);
    }
    else {           // Parent process       
        // Close the write end of the pipe
        close(pipefd[1]);

        // Read the output from the pipe
        char buffer[1024];
        std::wstring output;
        ssize_t bytesRead;

        // Create an iconv converter
        iconv_t conv = iconv_open("WCHAR_T", "UTF-8");
        if (conv == (iconv_t)-1) {
            perror("iconv_open");
            return L"iconv_open error!";
        }

        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            // Convert the input buffer from UTF-8 to wide characters
            char* inbuf = buffer;
            size_t inbytesleft = bytesRead;
            size_t outbytesleft = sizeof(buffer) * 4; // Wide characters can take up to 4 bytes

            char outbuf[4096]; // Adjust the buffer size as needed
            char* outptr = outbuf;

            if (iconv(conv, &inbuf, &inbytesleft, &outptr, &outbytesleft) == (size_t)-1) {
                perror("iconv");
                break;
            }

            // Calculate the number of wide characters
            size_t numWChars = (sizeof(buffer) * 4 - outbytesleft) / sizeof(wchar_t);

            if (numWChars > 0) {
                output.append(reinterpret_cast<wchar_t*>(outbuf), numWChars);
            }
        }

        // Close the iconv converter
        iconv_close(conv);

        // Close the read end of the pipe
        close(pipefd[0]);

        // Wait for the child process to exit
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exitStatus = WEXITSTATUS(status);
            std::wstringstream ss;
            ss << L" | Child process exited with status: " << exitStatus;
            output += ss.str();
        } else if (WIFSIGNALED(status)) {
            int signalNumber = WTERMSIG(status);
            std::wstringstream ss;
            ss << L" | Child process terminated due to signal: " << signalNumber;
            output += ss.str();
        }
        return output;
    }
}
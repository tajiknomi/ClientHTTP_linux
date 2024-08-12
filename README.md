## Intro
Lightweight C++ utility designed for Linux systems, aimed at enabling efficient remote administration of linux client machine. This tool is optimized for performance, small footprint and size. It interact with a REST/JSON server (*i.e. C&C server*) for communication.

## Application
Administer your linux client machine remotely via any REST/json server e.g. [Remote Administration Console](https://github.com/tajiknomi/Remote_Administrative_Console). It uses standard HTTP/json/base64 for communication, appearing as a standard web traffic for router/firewall(s). Whether you need to manage configurations, monitor system, execute commands, upload/download files or folders to/from machine, this app seamlessly integrates into your system without requiring additional software's.

## Features

***System Information***: Gather basic system information i.e. username, computer name, IP address, OS version etc.

***File Manager***: Effortlessly manage your files and directories such as view, copy, paste, and delete.

***Shell Handling***: This app will invoke ***/bin/sh*** on request which is available in almost every linux distribution. You can invoke other shells of your choice by using ***/bin/sh*** as the baseline.

***Upload/Download*** File or Directory: Seamlessly transfer files and directories to any http server (*which accepts files*).

***Archive/Compress File or Directory***: This feature compresses file/directory(s) before uploading it to the HTTP server.

***Execute***: Run program or scripts on the client to automate tasks.

***Notifications***: Stay informed with clear and concise messages or notifications from client about an operation or task.

***Logging***: Track client/server activity and events for troubleshooting.

***Persist (Optional)***: Implement your own persistence mechanism in "*src/operations.cpp::persist section*"

## Usage
For server side; you can use [this server app as a command & control unit](https://github.com/tajiknomi/Remote_Administrative_Console/releases/tag/v1.0.0) 

![Alt text](https://raw.githubusercontent.com/tajiknomi/Remote_Administrative_Console/main/screenshots/2.JPG)

or you can use your own REST/json http server.

Download the [clientHTTP_linux binaries]() from release section use the below command to communicate with the server.
```
./clientHTTP <URL/IP>
```
By default, the app will send hearbeat/alive signal every 1 sec in order to inform the server at *<URL/IP>* that it is alive and will collect the command/instruction from server (*if the server have any instruction/command/data for the client*). You can modify this interval time in main.cpp (variable --> *heartbeatTimerInSecs*).

The details of REST/json request/response are specified in the [REST requests (for advance users)](https://github.com/tajiknomi/Remote_Administrative_Console/blob/main/README.md#rest-requests-for-advance-users).


## How to build
The app is intentionally written on older version of linux i.e. [ubuntu 16.04 LTS](https://releases.ubuntu.com/16.04/) and build using [gcc 5.4.0](https://ftp.gnu.org/gnu/gcc/gcc-5.4.0/) for the purpose of backward compatibility.

Building this project requires the presence of two essential dependencies: [libcurl](https://github.com/curl/curl) and [RapidJSON](https://github.com/Tencent/rapidjson) (for efficient JSON parsing). Section of [RapidJSON](https://github.com/Tencent/rapidjson) which this app use is already accessible from my source code so you just need to install [libcurl](https://github.com/curl/curl) on your machine.

Build your curl library from [source](https://github.com/curl/curl) or use these easy to apply [commands](https://ec.haxx.se/install/linux.html) for your linux distribution.


```
cd clientHTTP_linux && mkdir build && cd build
cmake ../
make
```

you can also use cmake generators to build binary for Debug/Release and target architecture(s) for example.
```
cmake -DCMAKE_BUILD_TYPE=Debug ../
cmake -DTARGET_ARCH=x64 ../
cmake -DTARGET_ARCH=x86 ../
```

### For C++17 supporting compilers
Starting with [C++17 compatible compilers](https://en.cppreference.com/w/cpp/compiler_support/17), the [std::filesystem](https://en.cppreference.com/w/cpp/filesystem) is part of the standard library. If you are using C++17 or later, you may **NOT** need to link stdc++fs because the standard library will include filesystem support by default so in that case just remove the below in CMAKELists.txt file i.e.

```
target_link_libraries(clienthttp PRIVATE stdc++fs)
```
Also make sure to replace the replace the below lines in "*src/utilities.cpp*" and "*src/operations.cpp*"
```
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
```
with
```
#include <filesystem>
namespace fs = std::filesystem;
```

For other issues, please open thread in issue section.

### Disclaimer
This application is designed for personal and administrative use. It is not intended for unauthorized access, data manipulation, or any other malicious activity. Any use of this software for illegal purposes is strictly prohibited. You can use this service in offensive security scenarios on you own machine/network ONLY.
The author disclaims all liability for any misuse or damage caused by the application. Users are solely responsible for their actions and the consequences thereof.

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to add/change/modify.


## License

[MIT](https://opensource.org/license/mit)

#include <fstream>
#include <iostream>
#include "fileTransferService.h"
#include "curl/curl.h"
#include "stringUtil.h"

// ============================ PRIVATE FUNCTIONS ============================

size_t curlFileTransfer::WriteData(void* buffer, size_t size, size_t nmemb, void* userp) {
    std::ofstream* file = static_cast<std::ofstream*>(userp);
    file->write(static_cast<const char*>(buffer), size * nmemb);
    return size * nmemb;
}

size_t curlFileTransfer::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    return size * nmemb;
}

bool curlFileTransfer::isDataServerAvailable(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        // std::cerr << "Curl initialization failed." << std::endl;
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L); // Suppress output

    CURLcode res = curl_easy_perform(curl);
    long responseCode;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

    curl_easy_cleanup(curl);

    if (res == CURLE_OK && responseCode == 200) {
        return true;  // Port is open and responded with a 200 OK.
    }

    return false;  // Port is either closed or didn't respond as expected.
}



// ============================ PUBLIC API ============================

bool curlFileTransfer::DownloadFileFromURL(const std::wstring& url, const std::wstring& outputDirPath) {

    CURL* curl = curl_easy_init();
    if (!curl) {
        //std::cerr << "Failed to initialize libcurl" << std::endl;
        return false;
    }
    std::wstring outputFilePath = outputDirPath + L"/" + url.substr(url.find_last_of(L'/') + 1);
    std::ofstream outputFile(fs::path(outputFilePath), std::ios::binary);
    if (!outputFile) {
        std::wcerr << "Failed to open output file: " << outputFilePath << std::endl;
        curl_easy_cleanup(curl);
        return false;
    }
    std::string url_stdstring = StringUtils::ws2s(url);

    curl_easy_setopt(curl, CURLOPT_URL, url_stdstring.c_str());
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outputFile);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L); // Suppress output

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::wcerr << "Failed to download file: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        outputFile.close();
        std::error_code ec;
        fs::remove(outputFilePath.c_str(), ec);  // Remove the partially downloaded file
        return false;        
    }
    curl_easy_cleanup(curl);
    outputFile.close();
    return true;
}

bool curlFileTransfer::DownloadDirectoryFromURL(const std::wstring& url, const std::wstring& outputDirPath) {
    
    // To be implemented Later
    
    return false;
}

bool curlFileTransfer::UploadFileToURL(const std::wstring& url, const std::wstring& filePath) {

    // Open the file for reading
    std::ifstream fileStream(fs::path(filePath), std::ios::binary);
    if (!fileStream.is_open()) {
        // Handle file opening failure
        return false;
    }
    // Get the file size
    fileStream.seekg(0, std::ios::end);
    std::streampos fileSize = fileStream.tellg();

    if (fileSize == 0) { return false; } // Don't proceed if size is ZERO

    fileStream.seekg(0, std::ios::beg);

    // Allocate memory for the file content
    std::vector<char> fileBuffer(fileSize);

    // Read the file content into the buffer
    if (!fileStream.read(fileBuffer.data(), fileSize)) {
        // Handle file reading failure
        return false;
    }
    fileStream.close();

    // Extract file name
    std::wstring filename = StringUtils::extractFilename(filePath);

    // Convert the wstring filename to UTF-8 encoded C-style string
    std::string filenameUtf8 = StringUtils::convertWStringToUTF8(filename);

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "curl_easy_init() failed!" << std::endl;
        return false;
    }
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L); // Suppress output
    struct curl_httppost* formPost = nullptr;
    struct curl_httppost* lastPtr = nullptr;

    CURLFORMcode formAddResult = curl_formadd(&formPost, &lastPtr,
        CURLFORM_COPYNAME, "file",
        CURLFORM_BUFFER, filenameUtf8.c_str(),
        CURLFORM_BUFFERPTR, fileBuffer.data(),
        CURLFORM_BUFFERLENGTH, fileBuffer.size(),
        CURLFORM_END);

    if (formAddResult != CURL_FORMADD_OK) {
    //    errorMsg = L"Failed to add form data: " + StringUtils::s2ws(curl_easy_strerror((CURLcode)formAddResult));
        std::wcerr << L"Failed to add form data: " << StringUtils::s2ws(curl_easy_strerror((CURLcode)formAddResult)) << std::endl;
        curl_easy_cleanup(curl);
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formPost);
    curl_easy_setopt(curl, CURLOPT_URL, StringUtils::ws2s(url).c_str());

   // Capture server response [ If not used, the CURL will prompts the server response on STDOUT ]
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        return false;
    }
    curl_easy_cleanup(curl);
    return true;
}

bool curlFileTransfer::UploadDirectoryToURL(const std::wstring& url, const std::wstring& dirPath, std::wstring &errorMsg, const std::wstring& extensions) {

    if(!isDataServerAvailable(StringUtils::ws2s(url))){
        errorMsg = L"Couldn't connect to Data Server i.e " + url;
        return false;
    }
    std::vector<std::wstring> filesToUpload;
    std::vector<std::string> extensions_vec = StringUtils::extract_items_from_str(StringUtils::ws2s(extensions),",");
    std::error_code ec;
    for (const auto& entry : fs::recursive_directory_iterator(dirPath, fs::directory_options::skip_permission_denied, ec)) {
        fs::status(entry, ec).type();    // To get error_code status
        if(!ec) {
            if (fs::is_regular_file(entry, ec)) {            
                std::wstring filePath = entry.path().wstring();
                if (extensions.empty()) {
                    filesToUpload.push_back(filePath);
                } 
                else {
                    std::string extension = entry.path().extension().string();                
                    for(auto ext : extensions_vec){                
                        if(ext == extension){ 
                            filesToUpload.push_back(filePath);
                            break;
                        }
                    }
                }
            }
        }
        else { return false; }       
    }
    for (const auto& filePath : filesToUpload) {
        UploadFileToURL(url, filePath);
    }  
    return true;
}

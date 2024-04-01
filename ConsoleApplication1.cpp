#include <iostream>  
#include <string>  
#include <cstdlib>  
#include <cstdio>  
#include <windows.h>  
#include <wininet.h>  
#include <mysql.h>  
#include <vector>

#pragma comment(lib, "wininet.lib")  
#pragma comment(lib, "libmysql")  
#include <windows.h>  
#include <wininet.h>  
#include <string>  
#include <locale>  
#include <codecvt>  
#include <windows.h>  
#include <winhttp.h>  
#include <iostream>  
#include <string>  
#include <sstream>  

#pragma comment(lib, "winhttp.lib")  


#include <string>  
#include <vector>  
#include <sstream>  
#include <iomanip>  
#include <ctime>  
#include <cctype>  
#include <Windows.h>  
#include <Wininet.h>  
#pragma comment(lib, "wininet.lib")  
#include <iostream>  
#include <string>  
#include <vector>  
#include <sstream>  
#include <iomanip>  
#include <ctime>  
#include <Windows.h>  
#include <Wininet.h>  
#include <atlbase.h>  
#include <fstream> // 包含文件流操作的头文件
#include <iostream>  
#include <string>  
#include <vector>  
#include <sstream>  
#include <iomanip>  
#include <ctime>  
#include <cctype> // 包含 isdigit() 函数  
#pragma comment(lib, "wininet.lib") 

#pragma comment(lib, "wininet.lib")  

std::string sendHttpRequest111(const std::string& url) {
    HINTERNET hInternet = ::InternetOpen(L"MyApp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        throw std::runtime_error("Failed to open internet connection.");
    }

    HINTERNET hConnect = ::InternetOpenUrl(hInternet, CA2W(url.c_str()), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        ::InternetCloseHandle(hInternet);
        throw std::runtime_error("Failed to open URL.");
    }

    char buffer[4096];
    std::string response;
    DWORD bytesRead;
    while (::InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        response.append(buffer, bytesRead);
    }

    ::InternetCloseHandle(hConnect);
    ::InternetCloseHandle(hInternet);

    return response;
}


std::string GetIdxPxFromResponse(const std::string& response) {
    std::string key = "\"idxPx\":\"";
    size_t pos = response.find(key);
    if (pos == std::string::npos) {
        return "Key not found";
    }
    pos += key.length();

    size_t endPos = response.find("\"", pos);
    if (endPos == std::string::npos) {
        return "Value not found";
    }

    return response.substr(pos, endPos - pos);
}


std::wstring StringToWideString(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

std::string SendHttpRequest(const std::string& url) {
    // 转换url为宽字符字符串  
    std::wstring wideUrl = StringToWideString(url);

    // 初始化WinINet并设置HTTP请求    
    HINTERNET hInternet = InternetOpenW(L"MyHttpApp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        std::cerr << "InternetOpen failed." << std::endl;
        return "";
    }

    HINTERNET hConnect = InternetOpenUrlW(hInternet, wideUrl.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        std::cerr << "InternetOpenUrl failed." << std::endl;
        InternetCloseHandle(hInternet);
        return "";
    }

    char buffer[4096];
    DWORD bytesRead;
    std::string response;
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        response.append(buffer, bytesRead);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return response;
}

double ExtractSellTokenPriceInUsd(const std::string& json) {
    // 假设JSON格式是 {"sellTokenPriceInUsd": 1234.56}  
    // 在实际应用中，你应该使用JSON库来解析JSON数据  
    size_t pos = json.find("\"buyTokenPriceInUsd\":") + strlen("\"buyTokenPriceInUsd\":");
    size_t end = json.find(",", pos);
    if (end == std::string::npos) {
        end = json.length(); // 如果没有逗号，则取到字符串末尾  
    }
    std::string priceStr = json.substr(pos, end - pos);
    priceStr.erase(remove(priceStr.begin(), priceStr.end(), ' '), priceStr.end()); // 移除空格  
    return atof(priceStr.c_str()); // 转换为double类型  
}

bool InsertIntoDatabase(double sellTokenPriceInUsd) {
    MYSQL* con = mysql_init(NULL);
    if (con == NULL) {
        std::cerr << "mysql_init() failed" << std::endl;
        return false;
    }

    const char* server = "38.54.88.78";
    const char* user = "web";
    const char* password = "NnhsND3Rm6w3CNjY";
    const char* database = "web";
    unsigned int port = 3306;

    if (mysql_real_connect(con, server, user, password, database, port, NULL, 0) == NULL) {
        std::cerr << "Connect to database error: " << mysql_error(con) << std::endl;
        mysql_close(con);
        return false;
    }

    char query[1024];
    sprintf_s(query, "INSERT INTO hangqing_copy1 (ETH) VALUES (%f)", sellTokenPriceInUsd);

    if (mysql_query(con, query)) {
        std::cerr << "Insert error: " << mysql_error(con) << std::endl;
        mysql_close(con);
        return false;
    }

    mysql_close(con);
    return true;
}


double GetIdxPxFromApi_BTC() {
    HINTERNET hSession = WinHttpOpen(L"MyUserAgent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        std::cerr << "Error: WinHttpOpen failed." << std::endl;
        return 0.0;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, L"www.ouxyi.space", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        std::cerr << "Error: WinHttpConnect failed." << std::endl;
        WinHttpCloseHandle(hSession);
        return 0.0;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/api/v5/market/index-tickers?instId=BTC-USDT", NULL, NULL, NULL, WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        std::cerr << "Error: WinHttpOpenRequest failed." << std::endl;
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 0.0;
    }

    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        std::cerr << "Error: WinHttpSendRequest failed." << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 0.0;
    }

    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        std::cerr << "Error: WinHttpReceiveResponse failed." << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 0.0;
    }

    DWORD dwSize = 0;
    DWORD dwRead = 0;
    std::string response;
    char buffer[1024] = { 0 };
    while (WinHttpQueryDataAvailable(hRequest, &dwSize) && dwSize > 0) {
        if (!WinHttpReadData(hRequest, buffer, sizeof(buffer), &dwRead) || dwRead == 0) {
            break;
        }
        response.append(buffer, dwRead);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    size_t posStart = response.find("\"idxPx\":\"");
    if (posStart == std::string::npos) {
        std::cerr << "Error: Cannot find \"idxPx\" in the response." << std::endl;
        return 0.0;
    }
    posStart += strlen("\"idxPx\":\"");

    size_t posEnd = response.find("\"", posStart);
    if (posEnd == std::string::npos) {
        std::cerr << "Error: Cannot find the end of \"idxPx\" value." << std::endl;
        return 0.0;
    }

    std::string idxPxStr = response.substr(posStart, posEnd - posStart);
    double idxPx = std::stod(idxPxStr);

    std::cout << "OKX BTC: " << idxPx << std::endl;
    return idxPx;
}

double GetIdxPxFromApi_ETH() {
    HINTERNET hSession = WinHttpOpen(L"MyUserAgent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        std::cerr << "Error: WinHttpOpen failed." << std::endl;
        return 0.0;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, L"www.ouxyi.space", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        std::cerr << "Error: WinHttpConnect failed." << std::endl;
        WinHttpCloseHandle(hSession);
        return 0.0;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/api/v5/market/index-tickers?instId=ETH-USDT", NULL, NULL, NULL, WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        std::cerr << "Error: WinHttpOpenRequest failed." << std::endl;
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 0.0;
    }

    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        std::cerr << "Error: WinHttpSendRequest failed." << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 0.0;
    }

    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        std::cerr << "Error: WinHttpReceiveResponse failed." << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 0.0;
    }

    DWORD dwSize = 0;
    DWORD dwRead = 0;
    std::string response;
    char buffer[1024] = { 0 };
    while (WinHttpQueryDataAvailable(hRequest, &dwSize) && dwSize > 0) {
        if (!WinHttpReadData(hRequest, buffer, sizeof(buffer), &dwRead) || dwRead == 0) {
            break;
        }
        response.append(buffer, dwRead);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    size_t posStart = response.find("\"idxPx\":\"");
    if (posStart == std::string::npos) {
        std::cerr << "Error: Cannot find \"idxPx\" in the response." << std::endl;
        return 0.0;
    }
    posStart += strlen("\"idxPx\":\"");

    size_t posEnd = response.find("\"", posStart);
    if (posEnd == std::string::npos) {
        std::cerr << "Error: Cannot find the end of \"idxPx\" value." << std::endl;
        return 0.0;
    }

    std::string idxPxStr = response.substr(posStart, posEnd - posStart);
    double idxPx = std::stod(idxPxStr);

    std::cout << "OKX ETH: " << idxPx << std::endl;
    return idxPx;
}

double GetIdxPxFromApi_STRK() {
    HINTERNET hSession = WinHttpOpen(L"MyUserAgent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        std::cerr << "Error: WinHttpOpen failed." << std::endl;
        return 0.0;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, L"www.ouxyi.space", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        std::cerr << "Error: WinHttpConnect failed." << std::endl;
        WinHttpCloseHandle(hSession);
        return 0.0;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/api/v5/market/index-tickers?instId=STRK-USDT", NULL, NULL, NULL, WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        std::cerr << "Error: WinHttpOpenRequest failed." << std::endl;
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 0.0;
    }

    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        std::cerr << "Error: WinHttpSendRequest failed." << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 0.0;
    }

    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        std::cerr << "Error: WinHttpReceiveResponse failed." << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 0.0;
    }

    DWORD dwSize = 0;
    DWORD dwRead = 0;
    std::string response;
    char buffer[1024] = { 0 };
    while (WinHttpQueryDataAvailable(hRequest, &dwSize) && dwSize > 0) {
        if (!WinHttpReadData(hRequest, buffer, sizeof(buffer), &dwRead) || dwRead == 0) {
            break;
        }
        response.append(buffer, dwRead);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    size_t posStart = response.find("\"idxPx\":\"");
    if (posStart == std::string::npos) {
        std::cerr << "Error: Cannot find \"idxPx\" in the response." << std::endl;
        return 0.0;
    }
    posStart += strlen("\"idxPx\":\"");

    size_t posEnd = response.find("\"", posStart);
    if (posEnd == std::string::npos) {
        std::cerr << "Error: Cannot find the end of \"idxPx\" value." << std::endl;
        return 0.0;
    }

    std::string idxPxStr = response.substr(posStart, posEnd - posStart);
    double idxPx = std::stod(idxPxStr);
    std::cout << "OKX STRK: " << idxPx << std::endl;
    return idxPx;
}


double GetIdxPxFromApi_UNI() {
    HINTERNET hSession = WinHttpOpen(L"MyUserAgent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        std::cerr << "Error: WinHttpOpen failed." << std::endl;
        return 0.0;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, L"www.ouxyi.space", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        std::cerr << "Error: WinHttpConnect failed." << std::endl;
        WinHttpCloseHandle(hSession);
        return 0.0;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/api/v5/market/index-tickers?instId=UNI-USDT", NULL, NULL, NULL, WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        std::cerr << "Error: WinHttpOpenRequest failed." << std::endl;
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 0.0;
    }

    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        std::cerr << "Error: WinHttpSendRequest failed." << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 0.0;
    }

    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        std::cerr << "Error: WinHttpReceiveResponse failed." << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 0.0;
    }

    DWORD dwSize = 0;
    DWORD dwRead = 0;
    std::string response;
    char buffer[1024] = { 0 };
    while (WinHttpQueryDataAvailable(hRequest, &dwSize) && dwSize > 0) {
        if (!WinHttpReadData(hRequest, buffer, sizeof(buffer), &dwRead) || dwRead == 0) {
            break;
        }
        response.append(buffer, dwRead);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    size_t posStart = response.find("\"idxPx\":\"");
    if (posStart == std::string::npos) {
        std::cerr << "Error: Cannot find \"idxPx\" in the response." << std::endl;
        return 0.0;
    }
    posStart += strlen("\"idxPx\":\"");

    size_t posEnd = response.find("\"", posStart);
    if (posEnd == std::string::npos) {
        std::cerr << "Error: Cannot find the end of \"idxPx\" value." << std::endl;
        return 0.0;
    }

    std::string idxPxStr = response.substr(posStart, posEnd - posStart);
    double idxPx = std::stod(idxPxStr);

    std::cout << "OKX UNI: " << idxPx << std::endl;
    return idxPx;
}





#include <cstdlib> // 为了使用exit()  
 
#include <stdexcept> // 为了使用标准异常 


int main222() {

    try {

    // 定义两个URLs  
    std::vector<std::string> urls = {
        "https://starknet.api.avnu.fi/swap/v1/quotes?sellTokenAddress=0x053c91253bc9682c04929ca02ed00b3e423f6710d2ee7e0d5ebb06f3ecf368a8&buyTokenAddress=0x049d36570d4e46f48e99674bd3fcc84644ddd6b96f7c741b1562b82f9e004dc7&sellAmount=0x38d7e00&takerAddress=0x052D8E9778D026588a51595E30B0F45609B4F771EecF0E335CdeFeD1d84a9D89",
        "https://starknet.api.avnu.fi/swap/v1/quotes?sellTokenAddress=0x053c91253bc9682c04929ca02ed00b3e423f6710d2ee7e0d5ebb06f3ecf368a8&buyTokenAddress=0x4718f5a0fc34cc1af16a1cdee98ffb20c31f5cd61d6ab07201858f4287c938d&sellAmount=0x38d7e00&takerAddress=0x052D8E9778D026588a51595E30B0F45609B4F771EecF0E335CdeFeD1d84a9D89",
        "https://starknet.api.avnu.fi/swap/v1/quotes?sellTokenAddress=0x053c91253bc9682c04929ca02ed00b3e423f6710d2ee7e0d5ebb06f3ecf368a8&buyTokenAddress=0x03fe2b97c1fd336e750087d68b9b867997fd64a2661ff3ca5a7c771641e8e7ac&sellAmount=0x38d7e00&takerAddress=0x052D8E9778D026588a51595E30B0F45609B4F771EecF0E335CdeFeD1d84a9D89",
        "https://starknet.api.avnu.fi/swap/v1/quotes?sellTokenAddress=0x053c91253bc9682c04929ca02ed00b3e423f6710d2ee7e0d5ebb06f3ecf368a8&buyTokenAddress=0x49210ffc442172463f3177147c1aeaa36c51d152c1b0630f2364c300d4f48ee&sellAmount=0x38d7e00&takerAddress=0x052D8E9778D026588a51595E30B0F45609B4F771EecF0E335CdeFeD1d84a9D89",
         "https://starknet.api.avnu.fi/swap/v1/quotes?sellTokenAddress=0x053c91253bc9682c04929ca02ed00b3e423f6710d2ee7e0d5ebb06f3ecf368a8&buyTokenAddress=0x585c32b625999e6e5e78645ff8df7a9001cf5cf3eb6b80ccdd16cb64bd3a34&sellAmount=0x38d7e00&takerAddress=0x052D8E9778D026588a51595E30B0F45609B4F771EecF0E335CdeFeD1d84a9D89",
         "https://starknet.api.avnu.fi/swap/v1/quotes?sellTokenAddress=0x053c91253bc9682c04929ca02ed00b3e423f6710d2ee7e0d5ebb06f3ecf368a8&buyTokenAddress=0x124aeb495b947201f5fac96fd1138e326ad86195b98df6dec9009158a533b49&sellAmount=0x38d7e00&takerAddress=0x052D8E9778D026588a51595E30B0F45609B4F771EecF0E335CdeFeD1d84a9D89",
         "https://starknet.api.avnu.fi/swap/v1/quotes?sellTokenAddress=0x053c91253bc9682c04929ca02ed00b3e423f6710d2ee7e0d5ebb06f3ecf368a8&buyTokenAddress=0x1e0eee22c684fdf32babdd65e6bcca62a8ce2c23c8d5e68f3989595d26e1b4a&sellAmount=0x38d7e00&takerAddress=0x052D8E9778D026588a51595E30B0F45609B4F771EecF0E335CdeFeD1d84a9D89",
         "https://starknet.api.avnu.fi/swap/v1/quotes?sellTokenAddress=0x053c91253bc9682c04929ca02ed00b3e423f6710d2ee7e0d5ebb06f3ecf368a8&buyTokenAddress=0x0137dfca7d96cdd526d13a63176454f35c691f55837497448fad352643cfe4d4&sellAmount=0x38d7e00&takerAddress=0x052D8E9778D026588a51595E30B0F45609B4F771EecF0E335CdeFeD1d84a9D89"
   
    };
    // 定义对应的token名称  
    std::vector<std::string> tokens = { "ETH", "STRK","BTC","UNI","ZEND" ,"LODRS","STARKPEPE","AKU"};

    MYSQL* con = mysql_init(NULL);
    if (con == NULL) {
        std::cerr << "mysql_init() failed" << std::endl;
        return 1;
    }

    const char* server = "38.54.88.78";
    const char* user = "web";
    const char* password = "NnhsND3Rm6w3CNjY";
    const char* database = "web";
    unsigned int port = 3306;

    if (mysql_real_connect(con, server, user, password, database, port, NULL, 0) == NULL) {
        std::cerr << "Connect to database error: " << mysql_error(con) << std::endl;
        mysql_close(con);
        return 1;
    }
    std::vector<double> prices(tokens.size(), 0.0); // 初始化价格数组  

    for (size_t i = 0; i < urls.size(); ++i) {
        std::string response = SendHttpRequest(urls[i]);
        if (!response.empty()) {
            double sellTokenPriceInUsd = ExtractSellTokenPriceInUsd(response);
            std::cout << tokens[i] << " Token Price in USD: " << sellTokenPriceInUsd << std::endl;
            prices[i] = sellTokenPriceInUsd; // 存储价格  
        }
        else {
            std::cerr << "Failed to get response from the server for " << tokens[i] << "!" << std::endl;
            prices[i] = 0; // 假设-1表示价格获取失败  

            mysql_close(con);


            exit(0);
        }
    }

    // 构建并执行SQL插入语句  
    if (prices[0] != -1 && prices[1] != -1) {
        char query[1024];
        sprintf_s(query, sizeof(query), "INSERT INTO hangqing_copy1 (ETH, STRK,BTC,UNI,ZEND,LORDS,STARKPEPE,AKU,BTC_JIAOYISUO,ETH_JIAOYISUO,STRK_JIAOYISUO,UNI_JIAOYISUO) VALUES (%f, %f, %f, %f, %f, %f, %f, %.8f, %f, %f, %f, %f)", prices[0], prices[1], prices[2], prices[3], prices[4], prices[5], prices[6], prices[7], GetIdxPxFromApi_BTC(), GetIdxPxFromApi_ETH(), GetIdxPxFromApi_STRK(), GetIdxPxFromApi_UNI());

        if (mysql_query(con, query)) {
            std::cerr << "Insert error: " << mysql_error(con) << std::endl;
        }
        else {
            std::cout << "Data inserted into database successfully." << std::endl;
        }
    }
    else {
        std::cerr << "Failed to collect prices for all tokens!" << std::endl;
    }

    mysql_close(con);


    exit(0);
    }
    catch (const std::exception& e) {
       


        exit(0);
    }
    return 0;
}




int main() {

    main222();
    return 0;


    MYSQL* con = mysql_init(NULL);
    if (con == NULL) {
        std::cerr << "mysql_init() failed" << std::endl;
        return 1;
    }

    const char* server = "38.54.88.78";
    const char* user = "web";
    const char* password = "NnhsND3Rm6w3CNjY";
    const char* database = "web";
    unsigned int port = 3306;

    if (mysql_real_connect(con, server, user, password, database, port, NULL, 0) == NULL) {
        std::cerr << "Connect to database error: " << mysql_error(con) << std::endl;
        mysql_close(con);
        return 1;
    }


    std::ofstream outfile("output.txt"); // 创建文件输出流  
    if (!outfile.is_open()) {
        std::cerr << "Error: Unable to open output file." << std::endl;
        return 1;
    }

   // std::string url = "https://www.ouxyi.space/api/v5/market/history-index-candles?instId=UNI-USD&bar=1D";

    std::string url = "https://www.ouxyi.space/api/v5/market/history-index-candles?instId=UNI-USD&bar=1D&after=1625241600000";


    std::string response = sendHttpRequest111(url);

    size_t startPos = response.find("[[");
    size_t endPos = response.find("]]");
    if (startPos == std::string::npos || endPos == std::string::npos) {
        throw std::runtime_error("Invalid JSON format.");
    }

    std::string dataStr = response.substr(startPos + 2, endPos - startPos - 2);
    std::istringstream iss(dataStr);
    std::string timestampStr, openStr, highStr, lowStr, closeStr, volumeStr;

    std::cout << "Beijing Time\tPrice" << std::endl;
    outfile << "Beijing Time\tPrice" << std::endl;
    while (std::getline(iss, timestampStr, ',')) {
        std::getline(iss, openStr, ','); // We are interested in this  

        // Remove double quotes from openStr  
        openStr.erase(remove(openStr.begin(), openStr.end(), '"'), openStr.end());

        // Skip the rest of the line  
        std::string lineRemainder;
        std::getline(iss, lineRemainder, ']');

        // Convert UNIX timestamp to Beijing time  
        std::string cleanedTimestampStr;
        for (char ch : timestampStr) {
            if (std::isdigit(ch)) {
                cleanedTimestampStr.push_back(ch);
            }
        }

        uint64_t timestamp = std::stoull(cleanedTimestampStr);
        time_t t = (time_t)(timestamp / 1000);
        struct tm localTimeInfo = { 0 };
        localtime_s(&localTimeInfo, &t);
        localTimeInfo.tm_hour += 8; // Convert to Beijing time (UTC+8)  
        mktime(&localTimeInfo); // Normalize the time structure  

        // Print the time and price  
        std::cout << std::put_time(&localTimeInfo, "%Y-%m-%d %H:%M:%S") << "\t" << openStr << std::endl;
        outfile << std::put_time(&localTimeInfo, "%Y-%m-%d %H:%M:%S") << "\t" << openStr << std::endl;

        std::stringstream ss;
        ss << std::put_time(&localTimeInfo, "%Y-%m-%d %H:%M:%S");
        std::string timeStr = ss.str();


        

            char query[1024];
            sprintf(query, "INSERT INTO hangqing_copy1 (time_moren,UNI) VALUES ('%s', '%s')  ON DUPLICATE KEY UPDATE UNI='%s'  ", timeStr.c_str(), openStr.c_str(), openStr.c_str());


            if (mysql_query(con, query)) {
                std::cerr << "Insert error: " << mysql_error(con) << std::endl;
            }
            else {
                std::cout << "Data inserted into database successfully." << std::endl;
            }


        if (iss.peek() == ']') {
            break; // End of data  
        }
        iss.ignore(); // Ignore the comma separator  
    }

    return 0;
}
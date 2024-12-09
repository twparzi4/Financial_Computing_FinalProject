#include "Retriever.h"
void* myrealloc(void* ptr, size_t size)
{
	if (ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
}
int write_data(void* ptr, size_t size, size_t nmemb, void* data)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct* mem = (struct MemoryStruct*)data;
	mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
}

vector<string> GetDateRange(string DayZero, int N)
{
	/*
	Calculates the date range for a given center date and number of tradingdays required. 
	Strategy here is to generate a date range of 2N days before and after day zero, to ensure that enough data is obtained.
	*/

    int days_offset = static_cast<int> (2 * N);  // Modified 12-09: 1.5 * N seems to be not enough for N <46

    // Parse date string into a tm structure
    tm tm = {};
    istringstream ss(DayZero);
    ss >> get_time(&tm, "%Y-%m-%d");

    // convert tm to time_point
    time_t time = mktime(&tm);
    auto tp = chrono::system_clock::from_time_t(time);

    // calculate date range in time_point type
    auto lb = tp - chrono::hours(24 * days_offset);
    auto ub = tp + chrono::hours(24 * days_offset);

    // convert date boundaries back to string
    time_t lb_t = chrono::system_clock::to_time_t(lb);
    time_t ub_t = chrono::system_clock::to_time_t(ub);
    ostringstream lb_os;
    ostringstream ub_os;

    lb_os << put_time(localtime(&lb_t), "%Y-%m-%d");
    ub_os << put_time(localtime(&ub_t), "%Y-%m-%d");

    vector<string> vec;
    vec.push_back(lb_os.str());
    vec.push_back(ub_os.str());

    return vec;
}


// int Retriever::GetData(StocksGroup &stocks, int N)
// {
//     if (N < 40 || N > 80)
//     {
//         cout << "N is invalid." << endl;
//         return -1;
//     }
    
//     vector<string> boundaries;

//     // declare a pointer of CURL
//     CURL* handle;
//     CURLcode status;

//     // set up the program environment that libcurl needs
// 	curl_global_init(CURL_GLOBAL_ALL);

// 	// curl_easy_init() returns a CURL easy handle
// 	handle = curl_easy_init();

//     if (handle)
//     {
//         auto it = stocks.begin();
//         for(; it != stocks.end(); it++)
//         {
//             struct MemoryStruct data;
//             data.memory = NULL;
//             data.size = 0;

//             string symbol = it->first;

//             boundaries = GetDateRange(it->second.GetDayZero(), N);

//             // cout<<symbol<<endl;
//             // cout << boundaries[0] <<endl<<boundaries[1]<<endl<<it->second.GetDayZero()<<endl;

//             string url_request = url_common + symbol + ".US?" + "from=" + boundaries[0] + "&to=" + boundaries[1] + "&api_token=" + api_token + "&period=d";
//             curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());

//             //adding a user agent
//             curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
//             curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
//             curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

//             curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
//             curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

//             // retrieve data
//             status = curl_easy_perform(handle);

//             if (status != CURLE_OK)
//             {
//                 cout << "curl_easy_perform() failed: " << curl_easy_strerror(status) << endl;
//                 return -1;
//             }
            
//             // Pass historical price into stock
//             it->second.PassData(data);

//             // Warn if there's not enough data
//             // Clip redundate dates of data
//             it->second.Clipping(N);

//             // clear memory
//             free(data.memory);
//             data.size = 0;
//         }
        
//     }
//     else
// 	{
// 		cout << "Curl init failed!" << endl;
// 		return -1;
// 	}

// 	// cleanup what is created by curl_easy_init
// 	curl_easy_cleanup(handle);

// 	// release resources acquired by curl_global_init()
// 	curl_global_cleanup();

// 	return 0;
// }

// // Alex:改了一点
// int Retriever::GetData(StocksGroup& stocks, int N, Stock& iwv) {
//     if (N < 40 || N > 80) {
//         cout << "N is invalid." << endl;
//         return -1;
//     }

//     CURL* handle;
//     CURLcode status;
//     curl_global_init(CURL_GLOBAL_ALL);
//     handle = curl_easy_init();

//     if (!handle) {
//         cerr << "Curl init failed!" << endl;
//         return -1;
//     }

//     // Retrieve data for each stock in the group
//     for (auto it = stocks.begin(); it != stocks.end(); ++it) {
//         struct MemoryStruct data = {nullptr, 0};

//         string symbol = it->first;
//         vector<string> boundaries = GetDateRange(it->second.GetDayZero(), N);

//         string url_request = url_common + symbol + ".US?" + "from=" + boundaries[0] + "&to=" + boundaries[1] +
//                              "&api_token=" + api_token + "&period=d";
//         curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
//         curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0");
//         curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
//         curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
//         curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
//         curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

//         status = curl_easy_perform(handle);
//         if (status != CURLE_OK) {
//             cerr << "curl_easy_perform() failed for " << symbol << ": " << curl_easy_strerror(status) << endl;
//             free(data.memory);
//             continue;
//         }

//         // Pass historical price data to the stock
//         it->second.PassData(data);
//         it->second.Clipping(N);

//         free(data.memory);
//     }

//     // Retrieve data for IWV
//     struct MemoryStruct iwv_data = {nullptr, 0};

//     vector<string> iwv_boundaries = GetDateRange("2024-07-01", N); // Replace "2024-07-01" with actual date if needed
//     string iwv_url = url_common + "IWV.US?" + "from=" + iwv_boundaries[0] + "&to=" + iwv_boundaries[1] +
//                      "&api_token=" + api_token + "&period=d";
//     curl_easy_setopt(handle, CURLOPT_URL, iwv_url.c_str());
//     curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&iwv_data);

//     status = curl_easy_perform(handle);
//     if (status != CURLE_OK) {
//         cerr << "curl_easy_perform() failed for IWV: " << curl_easy_strerror(status) << endl;
//         free(iwv_data.memory);
//         curl_easy_cleanup(handle);
//         curl_global_cleanup();
//         return -1; // Indicate failure to retrieve IWV data
//     }

//     // Pass IWV data to the IWV Stock object
//     iwv.PassData(iwv_data);
//     iwv.Clipping(N);
//     cout << "Successfully retrieved IWV data." << endl;

//     free(iwv_data.memory);

//     curl_easy_cleanup(handle);
//     curl_global_cleanup();

//     return 0; // Success
// }



// Mutex for thread-safe access to the StocksGroup
mutex stocksMutex;

void Retriever::GetDataForStocksRange(StocksGroup::iterator start, StocksGroup::iterator end, int N) {
    CURL* handle;
    CURLcode status;
    curl_global_init(CURL_GLOBAL_ALL);
    handle = curl_easy_init();

    if (handle) {
        for (auto it = start; it != end; ++it) {
            struct MemoryStruct data;
            data.memory = NULL;
            data.size = 0;

            string symbol = it->first;
            vector<string> boundaries = GetDateRange(it->second.GetDayZero(), N);
            // cout << "Getting data for stock " << symbol << endl;
            string url_request = url_common + symbol + ".US?from=" + boundaries[0] + "&to=" + boundaries[1] +
                                 "&api_token=" + api_token + "&period=d";

            curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
            curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);

            status = curl_easy_perform(handle);
            if (status == CURLE_OK) {
                lock_guard<mutex> lock(stocksMutex);
                it->second.PassData(data);
                it->second.Clipping(N);
            } else {
                std::cerr << "Failed to retrieve data for " << symbol << std::endl;
            }

            free(data.memory);
        }
    }

    curl_easy_cleanup(handle);
    curl_global_cleanup();
}

// Modified code to suit benchmark need
int Retriever::GetData(StocksGroup &stocks, int N, Stock& iwv) {
    if (N < 40 || N > 80) {
        std::cerr << "N is invalid." << std::endl;
        return -1;
    }

    // Determine the number of hardware threads available for concurrent execution.
    // const size_t numThreads = std::thread::hardware_concurrency();
    // cout << numThreads << endl;  // it cannot go multithreading... number of threads is 1

    // Calculate the size of the workload each thread will handle
    // size_t chunkSize = stocks.size() / numThreads;
    const size_t numThreads = 3; // changed from 4 to 3 and no issues for extarcting stock price.. so it's the api's problem
    size_t chunkSize = stocks.size() / numThreads;

    vector<thread> threads;

    auto it = stocks.begin();
    for (size_t i = 0; i < numThreads; ++i) {
        auto start = it;
        advance(it, (i == numThreads - 1) ? stocks.size() - i * chunkSize : chunkSize);
        threads.emplace_back(&Retriever::GetDataForStocksRange, this, start, it, N);
    }

    for (auto &t : threads) {
        if (t.joinable()) t.join();
        // cout << "thread" << endl;
    }

    cout << "Start extracting data for benchmark" << endl;
    // Retrieve data for IWV
    CURL* handle;
    CURLcode status;
    curl_global_init(CURL_GLOBAL_ALL);
    handle = curl_easy_init();

    struct MemoryStruct iwv_data = {nullptr, 0};
    vector<string> iwv_boundaries = GetDateRange("2024-07-01", N); // Replace "2024-07-01" with actual date if needed
    string iwv_url = url_common + "IWV.US?" + "from=" + iwv_boundaries[0] + "&to=" + iwv_boundaries[1] +
                     "&api_token=" + api_token + "&period=d";
    curl_easy_setopt(handle, CURLOPT_URL, iwv_url.c_str());
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&iwv_data);
    
    status = curl_easy_perform(handle);
    if (status != CURLE_OK) {
        cerr << "curl_easy_perform() failed for IWV: " << curl_easy_strerror(status) << endl;
        free(iwv_data.memory);
        curl_easy_cleanup(handle);
        curl_global_cleanup();
        return -1; // Indicate failure to retrieve IWV data
    }
 
    // Pass IWV data to the IWV Stock object
    iwv.PassData(iwv_data);
    iwv.Clipping(N);
    cout << "Successfully retrieved IWV data." << endl;

    free(iwv_data.memory);

    curl_easy_cleanup(handle);
    curl_global_cleanup();

    return 0;
}

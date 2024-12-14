#pragma once
#include "Containers.h"
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include "curl/curl.h"
#include <sstream>
#include <locale>
#include <fstream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <thread>
#include <functional>
#include <mutex>

using namespace std;


class Retriever
{
private:
    string url_common;
    string api_token;


public:
    Retriever() : url_common("https://eodhistoricaldata.com/api/eod/"), api_token("6743c583842130.88976468") {}
    // Retriever() : url_common("https://eodhistoricaldata.com/api/eod/"), api_token("67492fc5c76d54.04894225") {}
    Retriever(string url_, string api_) : url_common(url_), api_token(api_) {}

    void GetDataForStocksRange(StocksGroup::iterator start, StocksGroup::iterator end, int N);

    //int GetData(StocksGroup &stocks, int N);  // Retrieve data and store it in stocks
    int GetData(StocksGroup &stocks, int N, Stock &iwv);
};

void* myrealloc(void* ptr, size_t size);
int write_data(void* ptr, size_t size, size_t nmemb, void* data);

vector<string> GetDateRange(string DayZero, int N);  // Get the natural date range satisfying 2N+1 working days 
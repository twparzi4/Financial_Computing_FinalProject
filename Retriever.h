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

using namespace std;

class Retriever
{
private:
    string url_common;
    string api_token;


public:
    Retriever() : url_common("https://eodhistoricaldata.com/api/eod/"), api_token("6743c583842130.88976468") {}
    Retriever(string url_, string api_) : url_common(url_), api_token(api_) {}

    int GetData(StocksGroup &stocks, int N);
};


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

void* myrealloc(void* ptr, size_t size)
{
	if (ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
}
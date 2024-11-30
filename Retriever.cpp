#include "Retriever.h"

int Retriever::GetData(StocksGroup &stocks, int N)
{
    if (N < 40 || N > 80)
    {
        cout << "N is invalid." << endl;
        return -1;
    }

    // declare a pointer of CURL
    CURL* handle;
    CURLcode status;

    // set up the program environment that libcurl needs
	curl_global_init(CURL_GLOBAL_ALL);

	// curl_easy_init() returns a CURL easy handle
	handle = curl_easy_init();

    if (handle)
    {
        auto it = stocks.begin();
        for(; it != stocks.end(); it++)
        {
            struct MemoryStruct data;
            data.memory = NULL;
            data.size = 0;

            string symbol = it->first;

            string url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
            curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());

            //adding a user agent
            curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

            // retrieve data
            status = curl_easy_perform(handle);

            if (status != CURLE_OK)
            {
                cout << "curl_easy_perform() failed: " << curl_easy_strerror(status) << endl;
                return -1;
            }
        }
        
    }
}
#include "Containers.h"
#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>
#include <fstream>
#include "curl/curl.h"

using namespace std;
const char *EarningAncmntFile = "Russell3000EarningsAnnouncements.csv";

void ExtractEarningsInfo(StocksGroup &TotalStocks)
{
    ifstream fin;
    fin.open(EarningAncmntFile, ios::in);
    string line, ticker, date, period_ending, estimate, reported, surprise, surprise_pct;

    // We need to skip the first row
    if (getline(fin, line))
    {
        cout << "Start to Read in Earning Informations." << endl;
    }
    while (!fin.eof())
    {
        getline(fin, line);
        stringstream sin(line);
        getline(sin, ticker, ',');
        getline(sin, date, ',');
        getline(sin, period_ending, ',');
        getline(sin, estimate, ',');
        getline(sin, reported, ',');
        getline(sin, surprise, ',');
        getline(sin, surprise_pct);

        // Initiate a stock instance and store information in TotalStock
        Stock stk(ticker, date, period_ending, stod(estimate), stod(reported), stod(surprise), stod(surprise_pct));
        TotalStocks[ticker] = stk;
    }
}

void PrintStockInfo(StocksGroup &S)
{
    string inputKey;

    while (true)
    {
        cout << "Enter a stock's ticker: ";
        getline(cin, inputKey);

        // If input is empty, quit
        if (inputKey.empty())
        {
            cout << "Exiting the application." << endl;
            break;
        }

        // Search for the key in the map
        auto it = S.find(inputKey);
        if (it != S.end())
        {
            Stock stk = it->second;
            cout << "Ticker: " << stk.GetTicker() << endl
                 << "Earnings announcement Date: " << stk.GetDayZero() << endl
                 << "Period Ending: " << stk.GetPeirodEnd() << endl
                 << "Estimated Earnings: " << stk.GetEsti() << endl 
                 << "Reported Earnings: " << stk.GetReported() << endl 
                 << "Surprise: " << stk.GetSurprise() << endl 
                 <<"Surprise%: " << stk.GetSurpricePct() << endl;
        }
        else
        {
            cout << inputKey << " not found." << endl;
        }
    }
}

int main()
{
    // Read in information from earnings announcement file
    StocksGroup TotalStock;
    ExtractEarningsInfo(TotalStock);

    // Test
    PrintStockInfo(TotalStock);

    return 0;
}
#include "Containers.h"
#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>
#include <fstream>
#include "curl/curl.h"
#include <algorithm>

using namespace std;
const char *EarningAncmntFile = "Russell3000EarningsAnnouncements.csv";

void ExtractEarningsInfo(StocksGroup &TotalStocks, vector<double> &surprises)
{
    // Extract stock info into Total Stocks, but push surprises data into a double vector for grouping
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
        // cout<<ticker<<endl;///
        getline(sin, date, ',');
        getline(sin, period_ending, ',');
        getline(sin, estimate, ',');
        getline(sin, reported, ',');
        getline(sin, surprise, ',');
        getline(sin, surprise_pct);

        // Initiate a stock instance and store information in TotalStock
        Stock stk(ticker, date, period_ending, stod(estimate), stod(reported), stod(surprise), stod(surprise_pct));
        surprises.push_back(stod(surprise_pct));
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
                 << "Surprise%: " << stk.GetSurprisePct() << endl << endl 
                 << "Data Status" << stk.GetStatus() << endl
                 << "Historical Price Information: " << endl;
            
            it->second.PrintHistoricalData();
        }
        else
        {
            cout << inputKey << " not found." << endl;
        }
    }
}

double calculatePercentile(vector<double>& data, double percentile) {
    if (data.empty() || percentile < 0.0 || percentile > 100.0) {
        throw invalid_argument("Invalid data or percentile.");
    }

    size_t n = data.size();
    size_t index = static_cast<size_t>(percentile / 100.0 * (n - 1));

    // Partially sort the vector to find the nth element
    nth_element(data.begin(), data.begin() + index, data.end());

    return data[index];
}


void StocksGrouping(StocksGroup &H, StocksGroup &M, StocksGroup &L, StocksGroup T, vector<double> &surprises)
{
    // Group the stocks based on its surprise
    // Calculate 33rd and 67th percentile for the surprises data
    double p33 = calculatePercentile(surprises, 33.0);
    double p66 = calculatePercentile(surprises, 66.0);

    auto it = T.begin();
    for(; it != T.end(); it++)
    {
        string tic = it->first;
        Stock stk = it->second;
        if (stk.GetSurprisePct() > p66)
        {
            stk.SetGroup("Beat Estimate Group");
            H[tic] = stk;
        }
        else if (stk.GetSurprisePct() <= p33)
        {
            stk.SetGroup("Miss Estimate Group");
            L[tic] = stk;
        }
        else 
        {
            stk.SetGroup("Meet Estimate Group");
            M[tic] = stk;
        }
    }
}
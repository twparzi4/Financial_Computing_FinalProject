/*
This class is to store information of individual stock.
Attributes:
ticker, report_date, period_ending, estimated, reported, surprise, surprise%
*/

#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <iostream>


using namespace std;

struct Row
{
    std::string date;
    double open, high, low, close, adj_close, volume;
};

typedef std::vector<struct Row> DataFrame;

struct MemoryStruct
{
    char* memory;
    size_t size;
};

class Stock
{
private:
    std::string ticker;
    std::string report_date;
    std::string period_ending;
    double estimated;
    double reported;
    double surprise;
    double surprise_pct;
    std::string group; // Store the group that the stock belongs to.

    std::stringstream sData; // Store raw historical price data for the stock.
    DataFrame df; // Store clipped historical data
    int data_status; // -1: data lack before day0; 1: data lack after day0; 0: no problem; -2: data lack efore and after day0
public:
    Stock() : ticker(""), report_date(""), period_ending(""), estimated(0.0), reported(0.0), surprise(0.0), surprise_pct(0.0), data_status(0) {}
    // Initialize with params
    Stock(std::string tic, std::string date, std::string end_date, double esti, double reported_, double surp, double surp_pct) : \
            ticker(tic), report_date(date), period_ending(end_date), estimated(esti), reported(reported_), surprise(surp), surprise_pct(surp_pct), data_status(0) {}
    
    // Getter functions
    std::string GetTicker() { return ticker; }
    std::string GetDayZero() { return report_date; }
    std::string GetPeirodEnd() { return period_ending; }
    double GetEsti() { return estimated; }
    double GetReported() { return reported; }
    double GetSurprise() { return surprise; }
    double GetSurprisePct() { return surprise_pct; }
    std::string GetGroup() { return group; }
    int GetStatus() { return data_status; }


    void PrintHistoricalData();
    void Clipping(int N);  // count how many effective rows of historical data is in sData

    // Setter function
    void SetGroup(std::string g) { group = g; }
    void PassData(struct MemoryStruct &source) { sData.str(source.memory); }

    // overload assignment operator
    Stock& operator=(const Stock &source);

    // overload copy constructor
    Stock(const Stock &source);
};


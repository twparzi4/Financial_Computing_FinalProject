/*
This class is to store information of individual stock.
Attributes:
ticker, report_date, period_ending, estimated, reported, surprise, surprise%
*/

#pragma once

#include <string>

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

public:
    Stock() : ticker(""), report_date(""), period_ending(""), estimated(0.0), reported(0.0), surprise(0.0), surprise_pct(0.0) {}
    // Initialize with params
    Stock(std::string tic, std::string date, std::string end_date, double esti, double reported_, double surp, double surp_pct) : \
            ticker(tic), report_date(date), period_ending(end_date), estimated(esti), reported(reported_), surprise(surp), surprise_pct(surp_pct) {}
    
    // Getter functions
    std::string GetTicker() { return ticker; }
    std::string GetDayZero() { return report_date; }
    std::string GetPeirodEnd() { return period_ending; }
    double GetEsti() { return estimated; }
    double GetReported() { return reported; }
    double GetSurprise() { return surprise; }
    double GetSurprisePct() { return surprise_pct; }
    std::string GetGroup() { return group; }

    // Setter function
    void SetGroup(std::string g) { group = g; }
};
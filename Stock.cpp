#include "Stock.h"
#include <cmath>
Stock& Stock::operator=(const Stock &source)
{
    if(this == &source)
    {
        return *this;
    }

    ticker = source.ticker;
    report_date = source.report_date;
    period_ending = source.period_ending;
    estimated = source.estimated;
    reported = source.reported;
    surprise = source.surprise;
    surprise_pct = source.surprise_pct;
    group = source.group;
    data_status = source.data_status;

    sData.str(source.sData.str());

    df.assign(source.df.begin(), source.df.end());

    return *this;
}

Stock::Stock(const Stock &source)
{
    ticker = source.ticker;
    report_date = source.report_date;
    period_ending = source.period_ending;
    estimated = source.estimated;
    reported = source.reported;
    surprise = source.surprise;
    surprise_pct = source.surprise_pct;
    group = source.group;
    data_status = source.data_status;

    sData.str(source.sData.str());

    df.assign(source.df.begin(), source.df.end());
}

// void Stock::Clipping(int N)
// {
//     string line, sDate;
//     double open, high, low, close, adjclose, volume;
//     int count_ttl = 0, count_before = 0, count_after = 0;
//     int meet_day0 = 0; // indicates have we encounter day zero through iterations
//     while (getline(sData, line))
//     {
//         size_t found = line.find('-');
//         if (found != string::npos)
//         {
//             count_ttl += 1;

//             sDate = line.substr(0, line.find_first_of(','));

//             if (sDate >= report_date && meet_day0 == 0) { meet_day0 = 1; }  // Earning announcement days are not necessarily trading days

//             if (sDate < report_date && meet_day0 == 0) { count_before += 1; }
//             else if (sDate > report_date && meet_day0 == 1) { count_after += 1; }
            

//             volume = stod(line.substr(line.find_last_of(',') + 1));
//             line.erase(line.find_last_of(','));

//             adjclose = stod(line.substr(line.find_last_of(',') + 1));
//             line.erase(line.find_last_of(','));

//             close = stod(line.substr(line.find_last_of(',') + 1));
//             line.erase(line.find_last_of(','));

//             low = stod(line.substr(line.find_last_of(',') + 1));
//             line.erase(line.find_last_of(','));

//             high = stod(line.substr(line.find_last_of(',') + 1));
//             line.erase(line.find_last_of(','));

//             open = stod(line.substr(line.find_last_of(',') + 1));
//             line.erase(line.find_last_of(','));

//             this->df.push_back({sDate, open, high, low, close, adjclose, volume});
//         }
//     }
//     // check if there is enough data both before and after day zero

//     // cout << ticker << " data status before checking: " << data_status << endl;


//     if (count_before < N)
//     {
//         cout << "Not enough data before day 0 for stock " << ticker << endl;
//         data_status = -1;
//         if (count_after < N)
//         {
//             cout <<"Not enough data before and after day 0 for stock" << ticker << endl;
//             data_status = -2;
//             return;
//         }
//         else
//         {
//             for (int i = 0; i < count_after - N; i++) { df.pop_back(); }
//             return;
//         }
//     }
//     else if (count_before >= N && count_after < N)
//     {
//         cout << "Not enough data after day 0 for stock " << ticker << endl;
//         data_status = 1;
//         for (int i = 0; i < count_before - N; i++) { df.erase(df.begin()); }
        
//         return;
//     }

//     // cout << "data status after checking: " << data_status << endl;
//     // data_status = 0;    

//     // clipping df
//     for (int i = 0; i < count_before - N; i++) { df.erase(df.begin()); }
//     for (int i = 0; i < count_after - N; i++) { df.pop_back(); }

//     return;
// }

void Stock::PrintHistoricalData()
{
    cout << "Date    \topen\thigh\tlow\tclose\tadj close\tvolume" << endl;
    for(auto it = df.begin(); it != df.end(); it++)
    {
        cout << it->date << "\t" << it->open << "\t" << it->high << "\t"
             << it->low << "\t" << it->close << "\t" << it->adj_close << "\t" << it->volume << endl;
    }
}

//改了一下
void Stock::Clipping(int N) {
    string line, sDate;
    double open, high, low, close, adjclose, volume;
    int count_before = 0, count_after = 0;
    bool encountered_day0 = false; // Tracks if DayZero has been encountered

    // Parse historical data
    while (getline(sData, line)) {
        size_t found = line.find('-');
        if (found != string::npos) {
            sDate = line.substr(0, line.find_first_of(','));
            volume = stod(line.substr(line.find_last_of(',') + 1));
            line.erase(line.find_last_of(','));
            adjclose = stod(line.substr(line.find_last_of(',') + 1));
            line.erase(line.find_last_of(','));
            close = stod(line.substr(line.find_last_of(',') + 1));
            line.erase(line.find_last_of(','));
            low = stod(line.substr(line.find_last_of(',') + 1));
            line.erase(line.find_last_of(','));
            high = stod(line.substr(line.find_last_of(',') + 1));
            line.erase(line.find_last_of(','));
            open = stod(line.substr(line.find_last_of(',') + 1));

            // Append row to DataFrame
            this->df.push_back({sDate, open, high, low, close, adjclose, volume});

            // Count rows before and after DayZero
            if (sDate < report_date && !encountered_day0) {
                ++count_before;
            } else if (sDate >= report_date && !encountered_day0) {
                encountered_day0 = true;
            } else if (sDate > report_date && encountered_day0) {
                ++count_after;
            }
        }
    }

    // Validate data sufficiency
    if (count_before < N || count_after < N) {
        cout << "Insufficient data for stock " << ticker << ": "
             << "before=" << count_before << ", after=" << count_after << endl;

        // Update data_status accordingly
        if (count_before < N && count_after < N) {
            data_status = -2; // Not enough data both before and after
        } else if (count_before < N) {
            data_status = -1; // Not enough data before
        } else if (count_after < N) {
            data_status = 1; // Not enough data after
        }
        return;
    }

    // Clip DataFrame to exactly `2N+1` rows
    while (count_before > N) {
        df.erase(df.begin());
        --count_before;
    }
    while (count_after > N) {
        df.pop_back();
        --count_after;
    }

    // Update status to indicate data is valid
    data_status = 0;
}


vector<double> Stock::CalculateDailyReturns() const {
    std::vector<double> returns;
    for (size_t i = 1; i < df.size(); ++i) {
        double return_val = log(df[i].adj_close / df[i - 1].adj_close);
        returns.push_back(return_val);
    }
    return returns;
}

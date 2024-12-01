#include "Stock.h"
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
    data_status = data_status;

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
    data_status = data_status;

    sData.str(source.sData.str());

    df.assign(source.df.begin(), source.df.end());
}

void Stock::Clipping(int N)
{
    string line, sDate;
    double open, high, low, close, adjclose, volume;
    int count_ttl = 0, count_before = 0, count_after = 0;
    int meet_day0 = 0; // indicates have we encounter day zero through iterations
    while (getline(sData, line))
    {
        size_t found = line.find('-');
        if (found != string::npos)
        {
            count_ttl += 1;

            sDate = line.substr(0, line.find_first_of(','));

            if (sDate >= report_date && meet_day0 == 0) { meet_day0 = 1; }  // Earning announcement days are not necessarily trading days

            if (sDate < report_date && meet_day0 == 0) { count_before += 1; }
            else if (sDate > report_date && meet_day0 == 1) { count_after += 1; }
            

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
            line.erase(line.find_last_of(','));

            this->df.push_back({sDate, open, high, low, close, adjclose, volume});
        }
    }
    // check if there is enough data both before and after day zero

    if (count_before < N)
    {
        cout << "Not enough data before day 0 for stock " << ticker << endl;
        data_status = -1;
        if (count_after < N)
        {
            cout <<"Not enough data before and after day 0 for stock" << ticker << endl;
            data_status = -2;
            return;
        }
        else
        {
            for (int i = 0; i < count_after - N; i++) { df.pop_back(); }
            return;
        }
    }
    else if (count_before >= N && count_after < N)
    {
        cout << "Not enough data after day 0 for stock " << ticker << endl;
        data_status = 1;
        for (int i = 0; i < count_before - N; i++) { df.erase(df.begin()); }
        
        return;
    }

    data_status = 0;    

    // clipping df
    for (int i = 0; i < count_before - N; i++) { df.erase(df.begin()); }
    for (int i = 0; i < count_after - N; i++) { df.pop_back(); }

    return;
}

void Stock::PrintHistoricalData()
{
    cout << "Date    \topen\thigh\tlow\tclose\tadj close\tvolume" << endl;
    for(auto it = df.begin(); it != df.end(); it++)
    {
        cout << it->date << "\t" << it->open << "\t" << it->high << "\t"
             << it->low << "\t" << it->close << "\t" << it->adj_close << "\t" << it->volume << endl;
    }
}
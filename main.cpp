#include "Containers.h"
#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>
#include <fstream>
#include "curl/curl.h"
#include "Functions.h"
#include "Retriever.h"

using namespace std;


int main()
{
    // Read in information from earnings announcement file
    StocksGroup TotalStock, BestEsti, MeetEsti, MissEsti;
    vector<double> surprises;
    ExtractEarningsInfo(TotalStock, surprises);
    
    Retriever loader;
    loader.GetData(TotalStock, 50);

    // auto it = TotalStock.begin();
    // int count = 0;
    // for(; it!=TotalStock.end(); ++it)
    // {
    //     cout<<it->second.GetTicker()<<endl;
    //     count += 1;
    //     if (count == 10)
    //     {
    //         break;
    //     }
    // }  // The first two iterations are empty key because the last two row of the earning announcement file is empty...
    


    // Group the stocks based on their earnings surprises
    // StocksGrouping(BestEsti, MeetEsti, MissEsti, TotalStock, surprises);

    // Test pulling earning information
    PrintStockInfo(TotalStock);

    // Test groups are around same size
    // cout << "Best size: " << BestEsti.size() << endl
    //      << "Miss size: " << MissEsti.size() << endl
    //      << "Meet size: " << MeetEsti.size() << endl;

    return 0;
}
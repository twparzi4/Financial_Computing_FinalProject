#include "Containers.h"
#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>
#include <fstream>
#include "curl/curl.h"
#include "Functions.h"

using namespace std;


int main()
{
    // Read in information from earnings announcement file
    StocksGroup TotalStock, BestEsti, MeetEsti, MissEsti;
    vector<double> surprises;
    ExtractEarningsInfo(TotalStock, surprises);

    // Group the stocks based on their earnings surprises
    StocksGrouping(BestEsti, MeetEsti, MissEsti, TotalStock, surprises);

    // Test pulling earning information
    // PrintStockInfo(TotalStock);

    // Test groups are around same size
    // cout << "Best size: " << BestEsti.size() << endl
    //      << "Miss size: " << MissEsti.size() << endl
    //      << "Meet size: " << MeetEsti.size() << endl;

    return 0;
}
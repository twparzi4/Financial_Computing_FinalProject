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
    // loader.GetData(TotalStock, 50);

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
    StocksGrouping(BestEsti, MeetEsti, MissEsti, TotalStock, surprises);

    // Test pulling earning information
    PrintStockInfo(TotalStock);

    // Test groups are around same size
    // cout << "Best size: " << BestEsti.size() << endl
    //      << "Miss size: " << MissEsti.size() << endl
    //      << "Meet size: " << MeetEsti.size() << endl;


    // Example CAAR data for three groups
    std::vector<double> caar_group1 = {0.01, 0.012, 0.014, 0.016, 0.018};
    std::vector<double> caar_group2 = {0.02, 0.021, 0.023, 0.025, 0.027};
    std::vector<double> caar_group3 = {-0.01, -0.008, -0.006, -0.004, -0.002};

    // Export data to file
    std::string data_file = "caar_data.dat";
    ExportCAARToFile(caar_group1, caar_group2, caar_group3, data_file);

    // Generate plot using gnuplot
    std::string script_file = "plot_caar.gp";
    PlotCAARWithGnuplot(data_file, script_file);


    return 0;
}
#include "Functions.h"
#include <iostream>

int main() {
    StocksGroup TotalStock, BestEsti, MeetEsti, MissEsti;
    vector<double> surprises;

    // Simulate grouping
    ExtractEarningsInfo(TotalStock, surprises);
    StocksGrouping(BestEsti, MeetEsti, MissEsti, TotalStock, surprises);

    try {
        // Randomly sample 30 stocks from the Best Estimate group
        StocksGroup sampled_stocks = BootstrapSample(BestEsti, 30);

        // Print the tickers of the sampled stocks
        cout << "Sampled Stocks from Best Estimate Group:\n";
        for (const auto& stock : sampled_stocks) {
            cout << stock.first << endl;
        }
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}

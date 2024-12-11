#include "Functions.h"
#include <fstream>
#include <sstream>
// #include <locale>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>


using namespace std;
const char *EarningAncmntFile = "Russell3000EarningsAnnouncements.csv";

void ExtractEarningsInfo(StocksGroup &TotalStocks)
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
        TotalStocks[ticker] = stk;
    }
}

void PrintStockInfo(StocksGroup &S)
{
    /*
    Print info of the stock that user inputsvoid ExportCAARToFile(const vector<double>& caar_group1, 
                      const vector<double>& caar_group2, 
                      const vector<double>& caar_group3, 
                      const string& filename, int N) {
    ofstream file(filename);

    if (!file.is_open()) {
        throw runtime_error("Failed to open file for writing CAAR data.");
    }

    int num_days = 2 * N + 1; // Correct range for days
    for (int i = 0; i < num_days; ++i) {
        int day_index = i - N; // Ensure indices range from -N to +N
        file << day_index << " "
             << caar_group1[i] << " "
             << caar_group2[i] << " "
             << caar_group3[i] << "\n";
    }

    file.close();
}
    */
    string inputKey;

    while (true)
    {
        cout << "Enter a stock's ticker: ";
        // Prompt to return to the menu or continue
        cout << "\n (If you want to return to the menu, press Enter. Otherwise, input the next ticker.)\n";
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
                 << "Data Status: " << stk.GetStatus() << endl
                 << "Historical Price Information: " << endl;
            
            it->second.PrintHistoricalData();
            // Prompt to return to the menu or continue
            cout << "\n(If you want to return to the menu, press Enter. Otherwise, input the next ticker.)\n";
        }
        else
        {
            cout << inputKey << " not found." << endl;
        }
    }
}

// double calculatePercentile(vector<double>& data, double percentile) {
//     if (data.empty() || percentile < 0.0 || percentile > 100.0) {
//         throw invalid_argument("Invalid data or percentile.");
//     }

//     size_t n = data.size();
//     size_t index = static_cast<size_t>(percentile / 100.0 * (n - 1));

//     // Partially sort the vector to find the nth element
//     nth_element(data.begin(), data.begin() + index, data.end());

//     return data[index];
// }


// void StocksGrouping(StocksGroup &H, StocksGroup &M, StocksGroup &L, StocksGroup T, vector<double> &surprises)
// {
//     // Group the stocks based on its surprise
//     // Calculate 33rd and 67th percentile for the surprises data
//     double p33 = calculatePercentile(surprises, 33.0);
//     double p66 = calculatePercentile(surprises, 66.0);

//     auto it = T.begin();
//     for(; it != T.end(); it++)
//     {
//         string tic = it->first;
//         Stock stk = it->second;
        
//         if (stk.GetSurprisePct() > p66)
//         {
//             stk.SetGroup("Beat Estimate Group");
//             H[tic] = stk;
//         }
//         else if (stk.GetSurprisePct() <= p33)
//         {
//             stk.SetGroup("Miss Estimate Group");
//             L[tic] = stk;
//         }
//         else 
//         {
//             stk.SetGroup("Meet Estimate Group");
//             M[tic] = stk;
//         }
//     }
// }

void StocksGrouping(StocksGroup& H, StocksGroup& M, StocksGroup& L, StocksGroup& T)
{
    vector<Pair> ValidStocks;
    for(auto& pair : T)
    {
        // Screen out stocks with abnormal data status
        if (pair.second.GetStatus() == 0) { ValidStocks.push_back(pair); }
    }

    // sort ValidStocks by surprise_pct
    sort(ValidStocks.begin(), ValidStocks.end(), 
        [](Pair& a, Pair& b) { return a.second.GetSurprisePct() < b.second.GetSurprisePct(); }); // ascending

    size_t one_third = ValidStocks.size() / 3;

    pair<string, Stock> temp;
    for (size_t i = 0; i < one_third; i++) { temp = ValidStocks[i]; L[temp.first] = temp.second; }
    for (size_t i = one_third; i < 2 * one_third; i++) {temp = ValidStocks[i]; M[temp.first] = temp.second; }
    for (size_t i = 2 * one_third; i< ValidStocks.size(); i++) { temp = ValidStocks[i]; H[temp.first] = temp.second; }

    // For checking purposes, print out the sizes of StocksGroups
    cout << "BeatEsti # of stocks: " << H.size() << endl;
    cout << "MeetEsti # of stocks: " << M.size() << endl;
    cout << "MissEsti # of stocks: " << L.size() << endl;
}


//Calculate AAR:
void CalculateAAR(const StocksGroup& group, const std::vector<double>& market_returns, int N, vector<double>& aar) {
    aar.assign(2 * N + 1, 0.0); // Initialize AAR vector
    size_t stock_count = group.size();

    for (const auto& stock_entry : group) {
        const Stock& stock = stock_entry.second;
        auto stock_returns = stock.CalculateDailyReturns();
        for (size_t t = 0; t < stock_returns.size(); ++t) {
            aar[t] += (stock_returns[t] - market_returns[t]) / stock_count; // Compute ARit and accumulate
        }
    }
}

//calculate CAAR: 
void CalculateCAAR(const std::vector<double>& aar, std::vector<double>& caar) {
    caar.resize(aar.size());
    caar[0] = aar[0];
    for (size_t t = 1; t < aar.size(); ++t) {
        caar[t] = caar[t - 1] + aar[t]; // Cumulative sum of AAR
    }
}



double Average(const std::vector<double>& data) {
    if (data.empty()) {
        throw std::invalid_argument("Cannot compute average of an empty vector.");
    }
    double sum = std::accumulate(data.begin(), data.end(), 0.0); // Sum all elements
    return sum / data.size(); // Divide by the number of elements
}


//Matrix Population: 
void PopulateMetricsMatrix(const vector<double>& aar, const vector<double>& caar,
                           const vector<double>& aar_std, const vector<double>& caar_std,
                           Matrix& metrics_matrix, int group_index) {
    if (group_index < 0 || group_index >= static_cast<int>(metrics_matrix.size())) {
        throw std::invalid_argument("Invalid group index for metrics matrix.");
    }

    metrics_matrix[group_index][0] = Average(aar);     // AAR
    metrics_matrix[group_index][1] = Average(aar_std); // AAR-STD
    metrics_matrix[group_index][2] = Average(caar);    // CAAR
    metrics_matrix[group_index][3] = Average(caar_std); // CAAR-STD
}



//Visualization of the Results (Matrix)
void PrintMetricsMatrix(const Matrix& metrics_matrix) {
    cout << "Metrics Matrix (Rows: Groups, Columns: AAR, AAR-STD, CAAR, CAAR-STD)\n";
    cout << "--------------------------------------------------------------\n";
    for (size_t i = 0; i < metrics_matrix.size(); ++i) {
        cout << "Group " << (i + 1) << ": ";
        for (const auto& value : metrics_matrix[i]) {
            cout << setw(10) << value << " ";
        }
        cout << "\n";
    }
}


// Export CAAR Data to a File
/*
void ExportCAARToFile(const vector<double>& caar_group1, 
                      const vector<double>& caar_group2, 
                      const vector<double>& caar_group3, 
                      const string& filename) {
    ofstream file(filename);

    if (!file.is_open()) {
        throw runtime_error("Failed to open file for writing CAAR data.");
    }

    size_t size = caar_group1.size();
    for (size_t i = 0; i < size; ++i) {
        file << i - size / 2 << " "  // Time index (e.g., -N, ..., 0, ..., N)
             << caar_group1[i] << " "
             << caar_group2[i] << " "
             << caar_group3[i] << "\n";
    }

    file.close();
}
*/

void ExportCAARToFile(const vector<double>& caar_group1, 
                      const vector<double>& caar_group2, 
                      const vector<double>& caar_group3, 
                      const string& filename, int N) {
    ofstream file(filename);

    if (!file.is_open()) {
        throw runtime_error("Failed to open file for writing CAAR data.");
    }

    int num_days = 2 * N + 1; // Correct range for days
    for (int i = 0; i < num_days; ++i) {
        int day_index = i - N + 1; // Ensure indices range from -N to +N
        file << day_index << " "
             << caar_group1[i] << " "
             << caar_group2[i] << " "
             << caar_group3[i] << "\n";
    }

    file.close();
}

// Plotting charts with gnuplot
void PlotCAARWithGnuplot(const std::string& data_file, const std::string& script_file) {
    std::string command = "gnuplot " + script_file;
    int result = std::system(command.c_str());
    if (result != 0) {
        throw std::runtime_error("Failed to execute gnuplot command.");
    }
}

// Dynamically generated gnuplot scripts
void WritePlotData(const string& filename, const vector<double>& group1,
                   const vector<double>& group2, const vector<double>& group3, int N) {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file for writing data.");
    }

    int num_days = 2 * N + 1;
    for (int i = 0; i < num_days; ++i) {
        int day_index = i - N + 1;
        file << day_index << " " << group1[i] << " " << group2[i] << " " << group3[i] << "\n";
    }

    file.close();
}


void WritePlotDataSingle(const string& filename, const vector<double>& group, int N) {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file for writing plot data.");
    }

    for (size_t i = 0; i < group.size(); ++i) {
        int day_index = static_cast<int>(i) - N;
        file << day_index << " " << group[i] << "\n";
    }

    file.close();
}

void GenerateGnuplotScript(const std::string& script_file, const std::string& data_file, 
                           const std::string& output_file, const std::string& title, 
                           const std::string& xlabel, const std::string& ylabel) {
    std::ofstream script(script_file);
    if (!script.is_open()) {
        throw std::runtime_error("Failed to create Gnuplot script file.");
    }

    script << "set terminal png size 800,600\n";
    script << "set output '" << output_file << "'\n";
    script << "set title '" << title << "'\n";
    script << "set xlabel '" << xlabel << "'\n";
    script << "set ylabel '" << ylabel << "'\n";
    script << "set grid\n";
    script << "plot '" << data_file << "' using 1:2 with lines title 'Group 1', \\\n";
    script << "     '" << data_file << "' using 1:3 with lines title 'Group 2', \\\n";
    script << "     '" << data_file << "' using 1:4 with lines title 'Group 3'\n";

    script.close();

    system(("gnuplot " + script_file).c_str());
}




void GenerateGnuplotScriptSingle(const string& data_file, const string& output_file,
                                 const string& title, const string& xlabel, const string& ylabel) {
    ofstream script("plot_single.gp");

    if (!script.is_open()) {
        throw runtime_error("Failed to create Gnuplot script file.");
    }

    script << "set terminal png size 800,600\n";
    script << "set output '" << output_file << "'\n";
    script << "set title '" << title << "'\n";
    script << "set xlabel '" << xlabel << "'\n";
    script << "set ylabel '" << ylabel << "'\n";
    script << "set grid\n";
    script << "plot '" << data_file << "' using 1:2 with lines title 'CAAR STD'\n";

    script.close();
    system("gnuplot plot_single.gp");
}




/*
void CreateGnuplotScript(const string& script_file, const string& data_file) {
    ofstream script(script_file);

    if (!script.is_open()) {
        cerr << "Error: Unable to create gnuplot script file." << endl;
        return;
    }

    script << "set terminal pngcairo size 800,600\n"
           << "set output 'caar_plot.png'\n"
           << "set title 'Cumulative Average Abnormal Returns (CAAR)'\n"
           << "set xlabel 'Days Relative to Earnings Announcement'\n"
           << "set ylabel 'CAAR'\n"
           << "set grid\n"
           << "plot '" << data_file << "' using 1:2 with lines title 'Beat Estimate Group',\\\n"
           << "     '" << data_file << "' using 1:3 with lines title 'Meet Estimate Group',\\\n"
           << "     '" << data_file << "' using 1:4 with lines title 'Miss Estimate Group'\n";

    script.close();
    cout << "Gnuplot script written to " << script_file << endl;
} */

// Randomly sample `sample_size` stocks from the input group
StocksGroup BootstrapSample(const StocksGroup& group, size_t sample_size) {
    StocksGroup sample;
    vector<StocksGroup::const_iterator> all_stocks;

    // Collect all stock iterators
    for (auto it = group.begin(); it != group.end(); ++it) {
        all_stocks.push_back(it);
    }
    // cout << all_stocks.size() << endl;
    // Check if the group size is smaller than the sample size
    if (all_stocks.size() < sample_size) {
        throw std::runtime_error("Group size is smaller than the requested sample size.");
    }

    // Randomly sample stocks
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, all_stocks.size() - 1);

    for (size_t i = 0; i < sample_size; ++i) {
        auto sampled_it = all_stocks[dist(gen)];
        sample[sampled_it->first] = sampled_it->second;
    }

    return sample;
}

//
// vector<double> ComputeStandardDeviation(const std::vector<std::vector<double>>& bootstrap_results, size_t num_days) {
    
//     vector<double> std_dev(num_days, 0.0);
//     for (size_t t = 0; t < num_days; ++t) {
//         double mean = 0.0;
//         double sum_squared_diff = 0.0;

//         for (const auto& sample : bootstrap_results) {
//             mean += sample[t];
//         }
//         mean /= bootstrap_results.size();

//         for (const auto& sample : bootstrap_results) {
//             sum_squared_diff += pow(sample[t] - mean, 2);
//         }

//         std_dev[t] = sqrt(sum_squared_diff / bootstrap_results.size());
//     }

//     return std_dev;
// }

Vector ComputeStandardDeviation(const Matrix& bs, size_t num_days)
{
    // Each element in Vector is a vector<double> of aar/caar series from one resample
    // We need to calculate std cross-sectional
    size_t num_bs = bs.size();  // Number of resample exercised
    double cross_sectional_mean, cross_sectional_sqmean;
    Vector stds;

    for (size_t i = 0; i != num_days; i++)
    {
        cross_sectional_mean = 0.0;
        cross_sectional_sqmean = 0.0;
        for (size_t j = 0; j != num_bs; j++)
        {
            cross_sectional_mean = (j * cross_sectional_mean + bs[j][i]) / (j + 1.0);
            cross_sectional_sqmean = (j * cross_sectional_sqmean + pow(bs[j][i], 2)) / (j + 1.0);
        }
        stds.push_back(sqrt(cross_sectional_sqmean - pow(cross_sectional_mean, 2)) / sqrt(num_bs - 1.0));
    }

    return stds;
}

tuple<Vector, Vector> Computations(const Matrix& bs, size_t num_days)
{
    // Each element in Vector is a vector<double> of aar/caar series from one resample
    // We need to calculate std cross-sectional
    size_t num_bs = bs.size();  // Number of resample exercised
    double cross_sectional_mean, cross_sectional_sqmean;
    Vector means, stds;

    for (size_t i = 0; i != num_days; i++)
    {
        cross_sectional_mean = 0.0;
        cross_sectional_sqmean = 0.0;
        for (size_t j = 0; j != num_bs; j++)
        {
            cross_sectional_mean = (j * cross_sectional_mean + bs[j][i]) / (j + 1.0);
            cross_sectional_sqmean = (j * cross_sectional_sqmean + pow(bs[j][i], 2)) / (j + 1.0);
        }
        means.push_back(cross_sectional_mean);
        stds.push_back(sqrt(cross_sectional_sqmean - pow(cross_sectional_mean, 2)) / sqrt(num_bs - 1.0));
    }

    return make_tuple(means, stds);
}

/*
void CalculateAAR_CAAR_Std(const StocksGroup& group, const std::vector<double>& market_returns, int N,
                           std::vector<double>& aar, std::vector<double>& caar,
                           std::vector<double>& aar_std, std::vector<double>& caar_std) {
    size_t num_bootstrap = 40; // Number of bootstrap samples
    size_t sample_size = 30;  // Number of stocks per sample
    size_t num_days = 2 * N + 1;

    // Containers for bootstrap AAR and CAAR results
    std::vector<std::vector<double>> bootstrap_aar(num_bootstrap, std::vector<double>(num_days, 0.0));
    std::vector<std::vector<double>> bootstrap_caar(num_bootstrap, std::vector<double>(num_days, 0.0));

    // Generate bootstrap samples and calculate AAR/CAAR for each sample
    for (size_t b = 0; b < num_bootstrap; ++b) {
        StocksGroup sample = BootstrapSample(group, sample_size);

        std::vector<double> sample_aar(num_days, 0.0);
        std::vector<double> sample_caar(num_days, 0.0);

        CalculateAAR(sample, market_returns, N, sample_aar);
        CalculateCAAR(sample_aar, sample_caar);

        // Store bootstrap results
        bootstrap_aar[b] = sample_aar;
        bootstrap_caar[b] = sample_caar;
    }

    // Calculate mean AAR and CAAR
    aar.assign(num_days, 0.0);
    caar.assign(num_days, 0.0);
    for (const auto& sample : bootstrap_aar) {
        for (size_t t = 0; t < num_days; ++t) {
            aar[t] += sample[t] / num_bootstrap;
        }
    }
    for (const auto& sample : bootstrap_caar) {
        for (size_t t = 0; t < num_days; ++t) {
            caar[t] += sample[t] / num_bootstrap;
        }
    }

    // Calculate standard deviations
    aar_std = ComputeStandardDeviation(bootstrap_aar, num_days);
    caar_std = ComputeStandardDeviation(bootstrap_caar, num_days);
}
*/

void CalculateAAR_CAAR_Std(const StocksGroup& group, const vector<double>& market_returns, int N,
                           vector<double>& aar, vector<double>& caar,
                           vector<double>& aar_std, vector<double>& caar_std) {
    size_t num_bootstrap = 40; // Define number of bootstrap iterations
    size_t sample_size = 30;   // Define sample size
    size_t num_days = 2 * N + 1;

    vector<vector<double>> bootstrap_aar(num_bootstrap, vector<double>(num_days, 0.0));
    vector<vector<double>> bootstrap_caar(num_bootstrap, vector<double>(num_days, 0.0));

    for (size_t b = 0; b < num_bootstrap; ++b) {
        StocksGroup sample = BootstrapSample(group, sample_size);

        vector<double> sample_aar(num_days, 0.0);
        vector<double> sample_caar(num_days, 0.0);

        CalculateAAR(sample, market_returns, N, sample_aar); // Calculate AAR
        CalculateCAAR(sample_aar, sample_caar);              // Calculate CAAR

        bootstrap_aar[b] = sample_aar;
        bootstrap_caar[b] = sample_caar;
    }

    tie(aar, aar_std) = Computations(bootstrap_aar, num_days); // Compute AAR and AAR-STD
    tie(caar, caar_std) = Computations(bootstrap_caar, num_days); // Compute CAAR and CAAR-STD
}

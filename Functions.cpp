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
    script << "plot '" << data_file << "' using 1:2 with lines title 'BeatEsti', \\\n";
    script << "     '" << data_file << "' using 1:3 with lines title 'MeetEsti', \\\n";
    script << "     '" << data_file << "' using 1:4 with lines title 'MissEsti'\n";

    script.close();

    system(("gnuplot " + script_file).c_str());
}

// void GenerateGnuplotScript(const std::string& script_file, const std::string& data_file,
//                            const std::string& title, const std::string& xlabel, 
//                            const std::string& ylabel) {
//     std::ofstream script(script_file);
//     if (!script.is_open()) {
//         throw std::runtime_error("Failed to create Gnuplot script file.");
//     }

//     // Configure Gnuplot script for interactive plotting
//     script << "set terminal wxt size 800,600\n";  // Use 'wxt' for interactive plotting
//     script << "set title '" << title << "'\n";
//     script << "set xlabel '" << xlabel << "'\n";
//     script << "set ylabel '" << ylabel << "'\n";
//     script << "set grid\n";
//     script << "plot '" << data_file << "' using 1:2 with lines title 'BeatEsti', \\\n";
//     script << "     '" << data_file << "' using 1:3 with lines title 'MeetEsti', \\\n";
//     script << "     '" << data_file << "' using 1:4 with lines title 'MissEsti'\n";

//     script.close();

//     // Execute the Gnuplot script
//     system(("gnuplot -persist " + script_file).c_str());
// }




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

        bootstrap_aar.push_back(sample_aar);
        bootstrap_caar.push_back(sample_caar);
    }

    tie(aar, aar_std) = Computations(bootstrap_aar, num_days); // Compute AAR and AAR-STD
    tie(caar, caar_std) = Computations(bootstrap_caar, num_days); // Compute CAAR and CAAR-STD
}

void PopulateMetricsMatrix(Matrix &metrics,
                           const vector<double> &aar_h, const vector<double> &caar_h,
                           const vector<double> &aar_std_h, const vector<double> &caar_std_h,
                           const vector<double> &aar_m, const vector<double> &caar_m,
                           const vector<double> &aar_std_m, const vector<double> &caar_std_m,
                           const vector<double> &aar_l, const vector<double> &caar_l,
                           const vector<double> &aar_std_l, const vector<double> &caar_std_l) {
    // Beat Estimate Group (Row 0)
    metrics[0][0] = accumulate(aar_h.begin(), aar_h.end(), 0.0) / aar_h.size();      // Average AAR
    metrics[0][1] = accumulate(aar_std_h.begin(), aar_std_h.end(), 0.0) / aar_std_h.size(); // Average AAR-STD
    metrics[0][2] = accumulate(caar_h.begin(), caar_h.end(), 0.0) / caar_h.size();   // Average CAAR
    metrics[0][3] = accumulate(caar_std_h.begin(), caar_std_h.end(), 0.0) / caar_std_h.size(); // Average CAAR-STD

    // Meet Estimate Group (Row 1)
    metrics[1][0] = accumulate(aar_m.begin(), aar_m.end(), 0.0) / aar_m.size();      // Average AAR
    metrics[1][1] = accumulate(aar_std_m.begin(), aar_std_m.end(), 0.0) / aar_std_m.size(); // Average AAR-STD
    metrics[1][2] = accumulate(caar_m.begin(), caar_m.end(), 0.0) / caar_m.size();   // Average CAAR
    metrics[1][3] = accumulate(caar_std_m.begin(), caar_std_m.end(), 0.0) / caar_std_m.size(); // Average CAAR-STD

    // Miss Estimate Group (Row 2)
    metrics[2][0] = accumulate(aar_l.begin(), aar_l.end(), 0.0) / aar_l.size();      // Average AAR
    metrics[2][1] = accumulate(aar_std_l.begin(), aar_std_l.end(), 0.0) / aar_std_l.size(); // Average AAR-STD
    metrics[2][2] = accumulate(caar_l.begin(), caar_l.end(), 0.0) / caar_l.size();   // Average CAAR
    metrics[2][3] = accumulate(caar_std_l.begin(), caar_std_l.end(), 0.0) / caar_std_l.size(); // Average CAAR-STD
}

void PlotMetricsWithGnuplot(const Matrix &metrics) {
    FILE *gnuplotPipe, *tempDataFile;
    const char *tempDataFileName = "metricsData";
    const char *title = "Metrics Matrix (AAR, AAR-STD, CAAR, CAAR-STD)";
    const char *xLabel = "Group";
    const char *yLabel = "Values";

    // Group names for x-axis labels
    const char *groups[3] = {"BeatEsti", "MeetEsti", "MissEsti"};

    gnuplotPipe = popen("gnuplot -persist", "w");
    if (gnuplotPipe) {
        // Write gnuplot configuration
        fprintf(gnuplotPipe, "set grid\n");
        fprintf(gnuplotPipe, "set title '%s'\n", title);
        fprintf(gnuplotPipe, "set xlabel '%s'\n", xLabel);
        fprintf(gnuplotPipe, "set ylabel '%s'\n", yLabel);
        fprintf(gnuplotPipe, "set style data histograms\n");
        fprintf(gnuplotPipe, "set style histogram cluster gap 1\n");
        fprintf(gnuplotPipe, "set style fill solid\n");
        fprintf(gnuplotPipe, "set boxwidth 0.9\n");
        fprintf(gnuplotPipe, "set xtics rotate by -45\n");

        // Write data to a temporary file
        tempDataFile = fopen(tempDataFileName, "w");
        if (!tempDataFile) {
            cerr << "Error opening temporary data file.\n";
            return;
        }

        // Write the matrix data to the temporary file
        fprintf(tempDataFile, "Group AAR AAR_STD CAAR CAAR_STD\n");
        for (int i = 0; i < 3; i++) {
            fprintf(tempDataFile, "%s %lf %lf %lf %lf\n", groups[i], 
                    metrics[i][0], metrics[i][1], metrics[i][2], metrics[i][3]);
        }
        fclose(tempDataFile);

        // Configure gnuplot to plot the data
        fprintf(gnuplotPipe, "plot for [col=2:5] '%s' using col:xtic(1) title columnheader(col) with histogram\n", tempDataFileName);
        fflush(gnuplotPipe);

        // Wait for user to close the plot
        cout << "Press Enter to close the plot...";
        getchar();

        // Clean up
        remove(tempDataFileName);
        fprintf(gnuplotPipe, "exit\n");
        pclose(gnuplotPipe);
    } else {
        cerr << "Gnuplot not found.\n";
    }
}



void PlotMetricsWithGnuplot(const Matrix3D &metrics, int N) {
    FILE *gnuplotPipe, *tempDataFile;
    const char *dataFileName = "metrics_data.dat";

    // Create a data file for gnuplot
    tempDataFile = fopen(dataFileName, "w");
    if (!tempDataFile) {
        cerr << "Failed to open temporary data file.\n";
        return;
    }

    // Write data into the file
    // Format: DayIndex AAR_Group1 AAR_STD_Group1 CAAR_Group1 CAAR_STD_Group1 ...
    for (int i = -N; i <= N; i++) {
        int index = i + N; // Shift index for vector access
        fprintf(tempDataFile, "%d ", i); // Day index
        for (const auto &group : metrics) {
            fprintf(tempDataFile, "%.6f %.6f %.6f %.6f ", 
                    group[index][0], // AAR
                    group[index][1], // AAR-STD
                    group[index][2], // CAAR
                    group[index][3]  // CAAR-STD
            );
        }
        fprintf(tempDataFile, "\n");
    }
    fclose(tempDataFile);

    // Open gnuplot pipe
    gnuplotPipe = popen("gnuplot -persist", "w");
    if (!gnuplotPipe) {
        cerr << "Failed to open gnuplot pipe.\n";
        return;
    }

    // Gnuplot commands for plot customization
    fprintf(gnuplotPipe, "set grid\n");
    fprintf(gnuplotPipe, "set title 'Metrics Plot'\n");
    fprintf(gnuplotPipe, "set xlabel 'Days Relative to Earnings Announcement'\n");
    fprintf(gnuplotPipe, "set ylabel 'Values'\n");
    fprintf(gnuplotPipe, "plot ");
    
    // Write plot commands for each metric
    const char *metricTitles[] = {"AAR BeatEsti", "AAR-STD BeatEsti", "CAAR BeatEsti", "CAAR-STD BeatEsti",
                                  "AAR MeetEsti", "AAR-STD MeetEsti", "CAAR MeetEsti", "CAAR-STD MeetEsti",
                                  "AAR MissEsti", "AAR-STD MissEsti", "CAAR MissEsti", "CAAR-STD MissEsti"};
    int columnIndex = 2; // Starting column for the first metric
    for (int i = 0; i < 12; ++i) {
        fprintf(gnuplotPipe, "'%s' using 1:%d with lines title '%s'", dataFileName, columnIndex++, metricTitles[i]);
        if (i < 11) {
            fprintf(gnuplotPipe, ", \\\n");
        } else {
            fprintf(gnuplotPipe, "\n");
        }
    }

    fflush(gnuplotPipe);
    cout << "Press Enter to close the plot...\n";
    getchar();

    // Cleanup
    remove(dataFileName);
    fprintf(gnuplotPipe, "exit\n");
    pclose(gnuplotPipe);
}

void plotResultsFromMatrix3D(const vector<vector<vector<double>>>& Matrix3D, int col_num, int N, const char* title) {
    FILE *gnuplotPipe, *tempDataFile;
    const char *tempDataFileName1 = "tempData1";
    const char *tempDataFileName2 = "tempData2";
    const char *tempDataFileName3 = "tempData3";

    // const char *title = "Metric Comparison Across Groups";
    const char *yLabel = "Metric Value";
    
    // Group names
    const char *group1 = "Beat Group";
    const char *group2 = "Meet Group";
    const char *group3 = "Miss Group";

    // Open Gnuplot pipe
    gnuplotPipe = popen("gnuplot -persist", "w");

    if (gnuplotPipe) {
        // Gnuplot configuration
        fprintf(gnuplotPipe, "set grid\n");
        fprintf(gnuplotPipe, "set title '%s'\n", title);
        fprintf(gnuplotPipe, "set xlabel 'Days Relative to Earnings Announcement'\n");
        fprintf(gnuplotPipe, "set ylabel '%s'\n", yLabel);
        fprintf(gnuplotPipe, "set arrow from 0,graph(0,0) to 0,graph(1,1) nohead lc rgb 'red'\n");
        fprintf(gnuplotPipe, "plot \"%s\" with lines title '%s', \"%s\" with lines title '%s', \"%s\" with lines title '%s'\n",
                tempDataFileName1, group1, tempDataFileName2, group2, tempDataFileName3, group3);

        fflush(gnuplotPipe);

        // Write data for each group to temporary files
        tempDataFile = fopen(tempDataFileName1, "w");
        for (int i = 0; i <= 2 * N; i++) {
            fprintf(tempDataFile, "%d %lf\n", -N + i, Matrix3D[0][col_num][i]); // Beat Group
        }
        fclose(tempDataFile);

        tempDataFile = fopen(tempDataFileName2, "w");
        for (int i = 0; i <= 2 * N; i++) {
            fprintf(tempDataFile, "%d %lf\n", -N + i, Matrix3D[1][col_num][i]); // Meet Group
        }
        fclose(tempDataFile);

        tempDataFile = fopen(tempDataFileName3, "w");
        for (int i = 0; i <= 2 * N; i++) {
            fprintf(tempDataFile, "%d %lf\n", -N + i, Matrix3D[2][col_num][i]); // Miss Group
        }
        fclose(tempDataFile);

        // Wait for user input before closing
        cout << "Press enter to continue...";
        getchar();

        // Cleanup temporary files
        remove(tempDataFileName1);
        remove(tempDataFileName2);
        remove(tempDataFileName3);

        // Exit Gnuplot
        fprintf(gnuplotPipe, "exit \n");
        pclose(gnuplotPipe);
    } else {
        cerr << "Gnuplot not found. Make sure it's installed and available in the PATH." << endl;
    }
}
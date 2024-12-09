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

int main() {
    // Containers for stocks and groups
    StocksGroup TotalStock, BestEsti, MeetEsti, MissEsti;
    vector<double> surprises;
    Stock iwv("IWV", "2024-07-01", "2024-07-01", 0.0, 0.0, 0.0, 0.0); // IWV stock object
    Matrix metrics(3, vector<double>(4, 0.0)); // 3 groups, 4 metrics: AAR, AAR-STD, CAAR, CAAR-STD
    vector<double> market_returns; // Placeholder for market returns

    int N = 50; // Default N value
    Retriever loader;

    while (true) {
        // Menu options
        cout << "\nMenu:\n"
             << "1. Enter N and retrieve historical price data\n"
             << "2. Pull information for one stock\n"
             << "3. Show AAR, AAR-STD, CAAR, and CAAR-STD for one group\n"
             << "4. Show gnuplot graph for CAAR\n"
             << "5. Exit\n"
             << "Enter your choice: ";

        int choice;
        cin >> choice;
        cin.ignore(); // To clear the newline character after input

        switch (choice) {
            case 1: {
                // Retrieve historical price data
                cout << "Enter N (40 <= N <= 80): ";
                cin >> N;
                cin.ignore();
                if (N < 40 || N > 80) {
                    cout << "Invalid value for N. Please try again.\n";
                    break;
                }

                // Extract earnings info and retrieve data
                ExtractEarningsInfo(TotalStock, surprises);
                if (loader.GetData(TotalStock, N, iwv) == -1) {
                    cout << "Failed to retrieve data for some stocks or IWV.\n";
                    break;
                }

                // Group stocks based on earnings surprises
                StocksGrouping(BestEsti, MeetEsti, MissEsti, TotalStock, surprises);
                market_returns = iwv.CalculateDailyReturns(); // Calculate IWV daily returns
                cout << "Data retrieved and grouped successfully.\n";
                break;
            }

            case 2: {
                // Pull information for one stock
                PrintStockInfo(TotalStock);
                break;
            }

            case 3: {
                // Show AAR, AAR-STD, CAAR, and CAAR-STD for one group
                int group_choice;
                cout << "Select a group:\n"
                     << "1. Beat Estimate Group\n"
                     << "2. Meet Estimate Group\n"
                     << "3. Miss Estimate Group\n"
                     << "Enter your choice: ";
                cin >> group_choice;
                cin.ignore();

                vector<double> aar, caar, aar_std, caar_std;
                StocksGroup selected_group;

                // Determine which group to use
                switch (group_choice) {
                    case 1:
                        selected_group = BestEsti;
                        break;
                    case 2:
                        selected_group = MeetEsti;
                        break;
                    case 3:
                        selected_group = MissEsti;
                        break;
                    default:
                        cout << "Invalid group choice.\n";
                        continue;
                }

                // Bootstrapping: Perform calculations for 40 samples
                size_t num_bootstrap = 40;
                for (size_t b = 0; b < num_bootstrap; ++b) {
                    // Randomly sample 30 stocks
                    try {
                        StocksGroup sampled_group = BootstrapSample(selected_group, 30);

                        // Calculate AAR and CAAR for the sampled group
                        vector<double> sample_aar, sample_caar;
                        CalculateAAR_CAAR_Std(sampled_group, market_returns, N, sample_aar, sample_caar, aar_std, caar_std);

                        // Aggregate results
                        for (size_t t = 0; t < sample_aar.size(); ++t) {
                            if (aar.size() <= t) aar.push_back(0.0);
                            if (caar.size() <= t) caar.push_back(0.0);
                            aar[t] += sample_aar[t] / num_bootstrap;
                            caar[t] += sample_caar[t] / num_bootstrap;
                        }

                    } catch (const std::exception& e) {
                        cerr << "Bootstrapping error: " << e.what() << endl;
                        continue;
                    }
                }

                // Populate metrics matrix with averaged results
                PopulateMetricsMatrix(aar, caar, aar_std, caar_std, metrics, group_choice - 1);

                PrintMetricsMatrix(metrics);
                break;
            }

            case 4: {
                // Show gnuplot graph for CAAR
                vector<double> caar_group1, caar_group2, caar_group3;

                // Perform bootstrapping and calculate CAAR for each group
                CalculateAAR_CAAR_Std(BestEsti, market_returns, N, caar_group1, caar_group1, caar_group1, caar_group1);
                CalculateAAR_CAAR_Std(MeetEsti, market_returns, N, caar_group2, caar_group2, caar_group2, caar_group2);
                CalculateAAR_CAAR_Std(MissEsti, market_returns, N, caar_group3, caar_group3, caar_group3, caar_group3);

                // Export data to file and plot
                string data_file = "caar_data.dat";
                ExportCAARToFile(caar_group1, caar_group2, caar_group3, data_file);

                string script_file = "plot_caar.gp";
                PlotCAARWithGnuplot(data_file, script_file);

                cout << "Graph generated using gnuplot.\n";
                break;
            }

            case 5:
                // Exit the program
                cout << "Exiting the program. Goodbye!\n";
                return 0;

            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
}

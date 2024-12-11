#include "Containers.h"
#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>
#include <fstream>
#include "curl/curl.h"
#include "Functions.h"
#include "Retriever.h"
#include <chrono>
#include <numeric>
#include "Matrix.h"



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
    // added
    std::vector<double> aar_group1 = {0.1, 0.2, 0.15, 0.05, 0.1, 0.05, -0.1, -0.15, 0.05, 0.1, 0.2};
    std::vector<double> aar_group2 = {0.05, 0.1, 0.08, 0.03, 0.07, 0.02, -0.05, -0.1, 0.02, 0.07, 0.12};
    std::vector<double> aar_group3 = {-0.05, -0.02, 0.01, 0.03, 0.04, -0.01, -0.02, -0.03, 0.01, 0.05, 0.08};
    
    std::vector<double> aar_std_group1 = {0.02, 0.03, 0.025, 0.01, 0.015, 0.02, 0.03, 0.025, 0.01, 0.015, 0.02};
    std::vector<double> aar_std_group2 = {0.01, 0.02, 0.018, 0.007, 0.012, 0.015, 0.02, 0.018, 0.007, 0.012, 0.015};
    std::vector<double> aar_std_group3 = {0.015, 0.025, 0.022, 0.008, 0.013, 0.018, 0.022, 0.019, 0.008, 0.013, 0.018};

    // CAAR and CAAR-STD would be populated similarly
    std::vector<double> caar_group1 = aar_group1; // Placeholder for example
    std::vector<double> caar_std_group1 = aar_std_group1; // Placeholder for example

    // Combine all groups into AllEsti
StocksGroup AllEsti = BestEsti;
AllEsti.insert(MeetEsti.begin(), MeetEsti.end());
AllEsti.insert(MissEsti.begin(), MissEsti.end());



    while (true) {
        // Menu options
        cout << "\nMenu:\n"
             << "1. Enter N and retrieve historical price data\n"
             << "2. Pull information for one stock\n"
             << "3. Show AAR, AAR-STD, CAAR, and CAAR-STD for one group\n"
             << "4. Show gnuplot graph for AAR\n"
             << "5. Show gnuplot graph for AAR-STD\n"
             << "6. Show gnuplot graph for CAAR\n"
             << "7. Show gnuplot graph for CAAR-STD\n"
             << "8. Exit\n"
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

                auto start_time = chrono::high_resolution_clock::now();
                // Extract earnings info and retrieve data
                ExtractEarningsInfo(TotalStock);
                if (loader.GetData(TotalStock, N, iwv) == -1) {
                    cout << "Failed to retrieve data for some stocks or IWV.\n";
                    break;
                }
                  
                auto end_time = chrono::high_resolution_clock::now();
                chrono::duration<double> elapsed = end_time - start_time;
                cout << "Time taken for retrieve data:" << elapsed.count() <<" seconds" << endl;

                // Group stocks based on earnings surprises
                StocksGrouping(BestEsti, MeetEsti, MissEsti, TotalStock);
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

                Vector aar, caar, aar_std, caar_std;
                Matrix aar_bs, caar_bs;  // Store results of bootstrap resamples. // Each element in matrix is the result series of a resample
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
                
                // cout << "size of the selected_group: " << selected_group.size();

                // Bootstrapping: Perform calculations for 40 samples
                size_t num_bootstrap = 40;
                for (size_t b = 0; b < num_bootstrap; ++b) {
                    // Randomly sample 30 stocks
                    // try {
                    //     StocksGroup sampled_group = BootstrapSample(selected_group, 30);

                    //     // Calculate AAR and CAAR for the sampled group
                    //     vector<double> sample_aar, sample_caar;
                    //     // CalculateAAR_CAAR_Std(sampled_group, market_returns, N, sample_aar, sample_caar, aar_std, caar_std);
                    //     CalculateAAR(sampled_group, market_returns, N, sample_aar);
                    //     CalculateCAAR(sample_aar, sample_caar);

                    //     // Aggregate results
                    //     // for (size_t t = 0; t < sample_aar.size(); ++t) {
                    //     //     if (aar.size() <= t) aar.push_back(0.0);
                    //     //     if (caar.size() <= t) caar.push_back(0.0);
                    //     //     aar[t] += sample_aar[t] / num_bootstrap;
                    //     //     caar[t] += sample_caar[t] / num_bootstrap;
                    //     // }

                    // } catch (const std::exception& e) {
                    //     cerr << "Bootstrapping error: " << e.what() << endl;
                    //     continue;
                    // }

                    StocksGroup sampled_group = BootstrapSample(selected_group, 30);

                    // Calculate AAR and CAAR for the sampled group
                    vector<double> sample_aar, sample_caar, aar_std, caar_std;
                    CalculateAAR(sampled_group, market_returns, N, sample_aar);
                    CalculateCAAR(sample_aar, sample_caar);
                    
                    aar_bs.push_back(sample_aar);
                    caar_bs.push_back(sample_caar);

                }
                
                // for (auto itr = aar_bs.begin(); itr != aar_bs.end(); itr++)
                // {
                //     double mu = accumulate((*itr).begin(), (*itr).end(), 0.0) / (2.0 * (double)N - 1.0);
                //     aar.push_back(mu);
                // }
                // for (auto itr = caar_bs.begin(); itr != caar_bs.end(); itr++)
                // {
                //     double mu = accumulate((*itr).begin(), (*itr).end(), 0.0) / (2.0 * (double)N - 1.0);
                //     caar.push_back(mu);
                // }
                

                // aar_std = ComputeStandardDeviation(aar_bs, 2 * N + 1);
                // caar_std = ComputeStandardDeviation(caar_bs, 2 * N + 1);
                tie(aar, aar_std) = Computations(aar_bs, 2 * N + 1);
                tie(caar, caar_std) = Computations(caar_bs, 2 * N + 1);

                cout << "size of aar: " << aar.size();
                cout << "size of caar: " << caar.size();

                // Populate metrics matrix with averaged results
                // PopulateMetricsMatrix(aar, caar, aar_std, caar_std, metrics, group_choice - 1);

                // PrintMetricsMatrix(metrics);
                cout << "AAR:" << endl << aar << endl;
                cout << "CAAR:" << endl << caar << endl;
                cout << "AAR_std:" << endl << aar_std << endl;
                cout << "CAAR_std:" << endl << caar_std << endl;


                break;
            }
/*
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
            } */

            case 4:
                WritePlotData("aar_data.dat", aar_group1, aar_group2, aar_group3, N);
                GenerateGnuplotScript("aar_plot.gp", "aar_data.dat", "aar_plot.png",
                      "Average Abnormal Returns (AAR)", "Days Relative to Earnings Announcement", "AAR");

                break;

            case 5:
                WritePlotData("aar_std_data.dat", aar_std_group1, aar_std_group2, aar_std_group3, N);
                GenerateGnuplotScript("aar_std_plot.gp", "aar_std_data.dat", "aar_std_plot.png",
                      "AAR Standard Deviation", "Days Relative to Earnings Announcement", "STD(AAR)");

                break;

            case 6: {
                vector<double> caar_group1, caar_group2, caar_group3;
                
                CalculateAAR_CAAR_Std(BestEsti, market_returns, N, caar_group1, caar_group1, caar_group1, caar_group1);
                CalculateAAR_CAAR_Std(MeetEsti, market_returns, N, caar_group2, caar_group2, caar_group2, caar_group2);
                CalculateAAR_CAAR_Std(MissEsti, market_returns, N, caar_group3, caar_group3, caar_group3, caar_group3);
                ExportCAARToFile(caar_group1, caar_group2, caar_group3, "caar_data.dat", N);
                GenerateGnuplotScript("caar_plot.gp", "caar_data.dat", "caar_plot.png",
                      "Cumulative Average Abnormal Returns (CAAR)", "Days Relative to Earnings Announcement", "CAAR");
                cout << "CAAR graph generated using Gnuplot.\n";
                break;
                }
                      
                case 7: {
                    vector<double> caar_group1, caar_group2, caar_group3;
                    vector<double> caar_std_group1, caar_std_group2, caar_std_group3;
                    CalculateAAR_CAAR_Std(BestEsti, market_returns, N, aar_group1, caar_group1, aar_std_group1, caar_std_group1);
                    CalculateAAR_CAAR_Std(MeetEsti, market_returns, N, aar_group2, caar_group2, aar_std_group2, caar_std_group2);
                    CalculateAAR_CAAR_Std(MissEsti, market_returns, N, aar_group3, caar_group3, aar_std_group3, caar_std_group3);
                    
                    WritePlotData("caar_std_data.dat", caar_std_group1, caar_std_group2, caar_std_group3, N);
                    GenerateGnuplotScript("caar_std_plot.gp", "caar_std_data.dat", "caar_std_plot.png",
                          "CAAR Standard Deviation", "Days Relative to Earnings Announcement", "STD(CAAR)");
                    cout << "CAAR-STD graph generated using Gnuplot.\n";
                    break;
                    }

            case 8:
                // Exit the program
                cout << "Exiting the program. Goodbye!\n";
                return 0;

            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
}

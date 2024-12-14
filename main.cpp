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
    Matrix3D metrics; // 3 groups, 4 metrics: AAR, AAR-STD, CAAR, CAAR-STD
    vector<double> market_returns; // Placeholder for market returns

    int N = 50; // Default N value
    Retriever loader;

    // Allocate Containers for CAAR and AAR
    Vector caar_h, caar_m, caar_l;
    Vector caar_std_h, caar_std_m, caar_std_l;
    Vector aar_h, aar_m, aar_l;
    Vector aar_std_h, aar_std_m, aar_std_l;


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
            //  << "4. Show plot.\n"
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
              
                // Calculate aar, caar for plotting
                CalculateAAR_CAAR_Std(BestEsti, market_returns, N, aar_h, caar_h, aar_std_h, caar_std_h);
                CalculateAAR_CAAR_Std(MeetEsti, market_returns, N, aar_m, caar_m, aar_std_m, caar_std_m);
                CalculateAAR_CAAR_Std(MissEsti, market_returns, N, aar_l, caar_l, aar_std_l, caar_std_l);

                Matrix Row1, Row2, Row3;
                Row1.push_back(aar_h); Row1.push_back(aar_std_h); Row1.push_back(caar_h); Row1.push_back(caar_std_h);
                Row2.push_back(aar_m); Row2.push_back(aar_std_m); Row2.push_back(caar_m); Row2.push_back(caar_std_m);
                Row3.push_back(aar_l); Row3.push_back(aar_std_l); Row3.push_back(caar_l); Row3.push_back(caar_std_l);

                metrics.push_back(Row1); metrics.push_back(Row2); metrics.push_back(Row3);
                
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
                   
                    StocksGroup sampled_group = BootstrapSample(selected_group, 30);

                    // Calculate AAR and CAAR for the sampled group
                    vector<double> sample_aar, sample_caar, aar_std, caar_std;
                    CalculateAAR(sampled_group, market_returns, N, sample_aar);
                    CalculateCAAR(sample_aar, sample_caar);
                    
                    aar_bs.push_back(sample_aar);
                    caar_bs.push_back(sample_caar);

                }
                
                tie(aar, aar_std) = Computations(aar_bs, 2 * N + 1);
                tie(caar, caar_std) = Computations(caar_bs, 2 * N + 1);

                cout << "size of aar: " << aar.size();
                cout << "size of caar: " << caar.size();


                cout << "AAR:" << endl << aar << endl;
                cout << "CAAR:" << endl << caar << endl;
                cout << "AAR_std:" << endl << aar_std << endl;
                cout << "CAAR_std:" << endl << caar_std << endl;


                break;
            }


            case 4:
                // WritePlotData("aar_data.dat", aar_h, aar_m, aar_l, N);
                // GenerateGnuplotScript("aar_plot.gp", "aar_data.dat", "aar_plot.png",
                //       "Average Abnormal Returns (AAR)", "Days Relative to Earnings Announcement", "AAR");
                plotResultsFromMatrix3D(metrics, 0, N, "AAR");

                break;

            case 5:
                // WritePlotData("aar_std_data.dat", aar_std_h, aar_std_m, aar_std_l, N);
                // GenerateGnuplotScript("aar_std_plot.gp", "aar_std_data.dat", "aar_std_plot.png",
                //       "AAR Standard Deviation", "Days Relative to Earnings Announcement", "STD(AAR)");
                plotResultsFromMatrix3D(metrics, 1, N, "AAR_std");

                break;

            case 6: {
                // ExportCAARToFile(caar_group1, caar_group2, caar_group3, "caar_data.dat", N);
                // WritePlotData("caar_data.dat", caar_h, caar_m, caar_l, N);
                // GenerateGnuplotScript("caar_plot.gp", "caar_data.dat", "caar_plot.png",
                //       "Cumulative Average Abnormal Returns (CAAR)", "Days Relative to Earnings Announcement", "CAAR");
                // cout << "CAAR graph generated using Gnuplot.\n";
                plotResultsFromMatrix3D(metrics, 2, N, "CAAR");
                break;
                }
                      
            case 7: {
                // WritePlotData("caar_std_data.dat", caar_std_h, caar_std_m, caar_std_l, N);
                // GenerateGnuplotScript("caar_std_plot.gp", "caar_std_data.dat", "caar_std_plot.png",
                //         "CAAR Standard Deviation", "Days Relative to Earnings Announcement", "STD(CAAR)");
                // cout << "CAAR-STD graph generated using Gnuplot.\n";
                plotResultsFromMatrix3D(metrics, 3, N, "CAAR_std");
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

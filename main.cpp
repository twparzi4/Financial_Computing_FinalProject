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
                CalculateAAR_CAAR_Std(BestEsti, market_returns, N, metrics);
                CalculateAAR_CAAR_Std(MeetEsti, market_returns, N, metrics);
                CalculateAAR_CAAR_Std(MissEsti, market_returns, N, metrics);
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

                cout << "AAR" << endl << metrics[group_choice][0];
                cout << "AAR_std" << endl << metrics[group_choice][1];
                cout << "CAAR" << endl << metrics[group_choice][2];
                cout << "CAAR_std" << endl << metrics[group_choice][3];


                break;
            }


            case 4:
                plotResultsFromMatrix3D(metrics, 0, N, "AAR");

                break;

            case 5:
                plotResultsFromMatrix3D(metrics, 1, N, "AAR_std");

                break;

            case 6: {

                plotResultsFromMatrix3D(metrics, 2, N, "CAAR");
                break;
                }
                      
            case 7: {
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

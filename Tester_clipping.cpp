#include "Containers.h"
#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>
#include <fstream>
#include "curl/curl.h"
#include "Functions.h"
#include "Retriever.h"
int main() {
    // Example stock data (replace with real data)
    Stock test_stock("AAPL", "2024-07-01", "2024-07-01", 1.5, 1.7, 0.2, 13.33);
    
    // Simulated raw historical data
    std::stringstream simulated_data(
        "2024-06-25,150.0,152.0,149.5,151.0,151.0,500000\n"
        "2024-06-26,151.0,153.0,150.0,152.0,152.0,450000\n"
        "2024-07-01,155.0,157.0,154.0,156.0,156.0,700000\n"
        
        "2024-07-02,156.0,158.0,155.0,157.0,157.0,650000\n"
    );

    // Pass simulated data to the stock
    struct MemoryStruct mock_data;
    mock_data.memory = strdup(simulated_data.str().c_str());
    mock_data.size = simulated_data.str().size();
    test_stock.PassData(mock_data);

    // Perform clipping
    test_stock.Clipping(2);

    // Print historical data after clipping
    test_stock.PrintHistoricalData();

    return 0;
}

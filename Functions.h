#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "Containers.h"
#include "Matrix.h"

extern const char* EarningAncmntFile;

void ExtractEarningsInfo(StocksGroup& TotalStocks, std::vector<double>& surprises);
void PrintStockInfo(StocksGroup& S);
double calculatePercentile(std::vector<double>& data, double percentile);
void StocksGrouping(StocksGroup& H, StocksGroup& M, StocksGroup& L, StocksGroup T, std::vector<double>& surprises);
void CalculateAAR(const StocksGroup& group, const std::vector<double>& market_returns, int N, std::vector<double>& aar);
void CalculateCAAR(const std::vector<double>& aar, std::vector<double>& caar);
double Average(const std::vector<double>& data);
void PopulateMetricsMatrix(const std::vector<double>& aar, const std::vector<double>& caar,
                           const std::vector<double>& aar_std, const std::vector<double>& caar_std,
                           Matrix& metrics_matrix, int group_index);
void PrintMetricsMatrix(const Matrix& metrics_matrix);
void ExportCAARToFile(const std::vector<double>& caar_group1, 
                      const std::vector<double>& caar_group2, 
                      const std::vector<double>& caar_group3, 
                      const std::string& filename);
void PlotCAARWithGnuplot(const std::string& data_file, const std::string& script_file);
StocksGroup BootstrapSample(const StocksGroup& group, size_t sample_size);
std::vector<double> ComputeStandardDeviation(const std::vector<std::vector<double>>& bootstrap_results, size_t num_days);
void CalculateAAR_CAAR_Std(const StocksGroup& group, const std::vector<double>& market_returns, int N,
                           std::vector<double>& aar, std::vector<double>& caar,
                           std::vector<double>& aar_std, std::vector<double>& caar_std);


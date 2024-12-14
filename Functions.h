#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <tuple>
#include "Containers.h"
#include "Matrix.h"

extern const char* EarningAncmntFile;

void ExtractEarningsInfo(StocksGroup& TotalStocks);
void PrintStockInfo(StocksGroup& S);
double calculatePercentile(std::vector<double>& data, double percentile);
void StocksGrouping(StocksGroup& H, StocksGroup& M, StocksGroup& L, StocksGroup& T);
void CalculateAAR(const StocksGroup& group, const std::vector<double>& market_returns, int N, std::vector<double>& aar);
void CalculateCAAR(const std::vector<double>& aar, std::vector<double>& caar);


void PlotCAARWithGnuplot(const std::string& data_file, const std::string& script_file);
StocksGroup BootstrapSample(const StocksGroup& group, size_t sample_size);
std::tuple<Vector, Vector> Computations(const Matrix& bs, size_t num_days);
void CalculateAAR_CAAR_Std(const StocksGroup& group, const vector<double>& market_returns, int N,
                           vector<double>& aar, vector<double>& caar,
                           vector<double>& aar_std, vector<double>& caar_std);


void WritePlotData(const string& filename, const vector<double>& group1, const vector<double>& group2,
                   const vector<double>& group3, int N);

void GenerateGnuplotScript(const std::string& script_file, const std::string& data_file, 
                           const std::string& output_file, const std::string& title, 
                           const std::string& xlabel, const std::string& ylabel);

void PlotMetricsWithGnuplot(const Matrix3D &metrics, int N);
void plotResultsFromMatrix3D(const vector<vector<vector<double>>>& Matrix3D, int col_num, int N, const char* title);



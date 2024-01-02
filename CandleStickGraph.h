#pragma once

#include <vector>
#include <string>
#include <array>
#include "CandleStick.h" // Assuming this is a custom class you've defined elsewhere

class CandleStickGraph {
public:
    // Constructor and destructor if needed
    // CandleStickGraph();
    // ~CandleStickGraph();

    // Member function declarations
    void stringBuilder(double step, double low, CandleStick& candle, std::array<std::string, 24>& strings);
    int getPrecision(std::string product);
    void buildCandlestick(std::vector<CandleStick> candlesticks, std::string productType);
    void volumeBuilder(double step, CandleStick& candle, std::array<std::string, 8>& strings);
    void volumeGraph(std::vector<CandleStick> candlesticks, std::string productType);

private:
    // Any private member variables or functions
};

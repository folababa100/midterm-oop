#pragma once


#include <string>
#include "OrderBook.h"

class CandleStick {
public:
    std::string date;
    double open;
    double high;
    double low;
    double close;
    double volume;

    CandleStick(std::string _date, double _open, double _high, double _low, double _close, double _volume)
            : date(_date), open(_open), high(_high), low(_low), close(_close), volume(_volume){}

    static std::vector<CandleStick> computeCandlesticks(OrderBook &orderBook, const std::string &productType);

};

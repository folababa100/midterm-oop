#include "CandleStick.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "OrderBook.h"
#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <ctime>
#include <stdexcept>

// I wrote this function below.
OrderBookType convert(const std::string& str)
{
    static const std::map<std::string, OrderBookType> mapping = {
      {"bid", OrderBookType::bid},
      {"ask", OrderBookType::ask},
      {"asksale", OrderBookType::asksale},
      {"bidsale", OrderBookType::bidsale}
    };

    auto it = mapping.find(str);
    if (it != mapping.end()) {
      return it->second;
    } else {
      return OrderBookType::unknown;
    }
}

/**epoch conversion utility**/
int epochConversion(const std::string& timestamp){
    std::vector<std::string> timeTokens = CSVReader::tokenise(timestamp, ' ');//split into year and timestamp
    std::vector<std::string> timeSet = CSVReader::tokenise(timeTokens[1],':');

    int hours = std::stoi(timeSet[0]);     // Convert to integer
    int minutes = std::stoi(timeSet[1]);
    int seconds = std::stoi(timeSet[2]);

    //convert things to int types
    struct tm timeThing = {0};
    timeThing.tm_year = 0;
    timeThing.tm_mon = 0;
    timeThing.tm_mday = 0;
    timeThing.tm_hour = hours;
    timeThing.tm_min = minutes;
    timeThing.tm_sec = seconds;

    time_t epoch = mktime(&timeThing);//yay now it's standardized

    return epoch;
}

//std::vector<CandleStick> computeCandlesticks(OrderBook& orderBook, const std::string& productType) {
/**candlestick computation, takes orderBook and a product type string as input and returns vector of candlestick objects**/
std::pair<std::string, OrderBookType> parseProductType(const std::string& productType) {
    std::vector<std::string> tokens = CSVReader::tokenise(productType, ',');
    if (tokens.size() != 2) {
        throw std::invalid_argument("Invalid product type format: " + productType);
    }
    return {tokens[0], convert(tokens[1])};
}

std::vector<CandleStick> CandleStick::computeCandlesticks(OrderBook& orderBook, const std::string& productType) {
    if (orderBook.getOrderSetSize() == 0) {
        throw std::runtime_error("Order book is empty.");
    }

    std::string product;
    OrderBookType type;
    std::tie(product, type) = parseProductType(productType);

    std::vector<CandleStick> candlesticks;
    OrderBook currentTimeBlockOrders;

    double runningTotal = 0;
    double counter = 0;
    double open = 0;
    double close = 0;
    double low = 0;
    double high = 0;
    bool isFirst = true;
    double volume = 0;

    OrderBookEntry lastOrder = orderBook.getOrderByIndex(0);

    for(int i = 0; i < orderBook.getOrderSetSize(); i++) {
        OrderBookEntry order = orderBook.getOrderByIndex(i);

        if(OrderBookEntry::compareByTimestamp(lastOrder, order)) {
            if (isFirst) {
                close = open = runningTotal / counter;
                isFirst = false;
            } else {
                close = runningTotal / counter;
                low = currentTimeBlockOrders.getLowPrice();
                high = currentTimeBlockOrders.getHighPrice();
                candlesticks.emplace_back(CandleStick(order.timestamp, open, high, low, close, volume));
                lastOrder = order;
                runningTotal = volume = counter = 0;
                open = close;
                currentTimeBlockOrders.removeAllElements();
            }
        } else {
            if(order.orderType == type && order.product == product) {
                double totalOrderValue = order.price * order.amount;
                runningTotal += totalOrderValue;
                volume += order.price;
                counter += order.amount;
                currentTimeBlockOrders.insertOrder(order);
            }
        }
    }
    return candlesticks;
}


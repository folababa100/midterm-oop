#include <vector>
#include <iostream>
#include <string>
#include <array>
#include "CandleStickGraph.h"
#include "CandleStick.h"
#include <sstream>
#include <iomanip>

/**stringbuilder that takes in the step of the graph, low point, candlestick, and an empty string array to set**/
void CandleStickGraph::stringBuilder(double step, double low, CandleStick& candle, std::array<std::string, 24>& strings) {
    const std::string GREEN_COLOR = "\033[32m";
    const std::string RED_COLOR = "\033[31m";
    const std::string RESET_COLOR = "\033[0m";
    const std::string EMPTY_SPACE = "              ";
    const std::string VERTICAL_LINE = "      |       ";
    const std::string CANDLE_BODY = "    ==|==     ";
    const std::string CANDLE_TOP_BOTTOM = "     ___      ";

    int openPos = int((candle.open - low)/step);
    int closePos = int((candle.close - low)/step);
    int highPos = int((candle.high - low)/step);
    int lowPos = int((candle.low - low)/ step);

    std::string color = (candle.close < candle.open) ? RED_COLOR : GREEN_COLOR;

    for (int i = 0; i < lowPos; ++i) {
        strings[i].append(EMPTY_SPACE);
    }
    for (int i = lowPos; i < std::min(openPos, closePos); ++i) {
        strings[i] += color + VERTICAL_LINE + RESET_COLOR;
    }
    strings[std::min(openPos, closePos)] += color + CANDLE_TOP_BOTTOM + RESET_COLOR;
    for (int i = std::min(openPos, closePos) + 1; i < std::max(openPos, closePos); ++i) {
        strings[i] += color + CANDLE_BODY + RESET_COLOR;
    }
    if (openPos != closePos) {
        strings[std::max(openPos, closePos)] += color + CANDLE_TOP_BOTTOM + RESET_COLOR;
    }
    for (int i = std::max(openPos, closePos) + 1; i <= highPos; ++i) {
        strings[i] += color + VERTICAL_LINE + RESET_COLOR;
    }
    for (int i = highPos + 1; i < 16; ++i) {
        strings[i] += EMPTY_SPACE;
    }
}

/**get precision for the graph, takes a product string**/
int CandleStickGraph::getPrecision(std::string product){
    int precisionSize = 2;
    if(product == "ETH/BTC"){
        precisionSize = 7;
    }
    else if(product == "DOGE/BTC"){
        precisionSize = 8;
    }
    else if(product == "BTC/USDT"){
        precisionSize = 2;
    }
    else if(product == "ETH/USDT"){
        precisionSize = 4;
    }
    else if(product == "DOGE/USDT"){
        precisionSize = 6;
    }
    else{
        precisionSize = 2;
    }

    return precisionSize;
}

/**actual candlestick construction, takes a vector of candlesticks and the producttype**/
void CandleStickGraph::buildCandlestick(std::vector<CandleStick> candlesticks, std::string productType){
    //calculate the highest and lowest values for each candlestick
    CandleStick highest_candlestick = *std::max_element(candlesticks.begin(), candlesticks.end(), [](CandleStick a, CandleStick b){return a.high < b.high;});
    CandleStick lowest_candlestick = *std::min_element(candlesticks.begin(), candlesticks.end(), [] (CandleStick a, CandleStick b){return a.low < b.low;});

    //set the product string
    std::string product;


    std::vector<std::string> tokens = CSVReader::tokenise(productType, ',');
    if (tokens.size() != 2)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << productType << std::endl;
    }
    else{
        //product name
        product = tokens[0];
    }



    int precisionSize = getPrecision(product);

    //long double step = (high-low)/15;

    double high = highest_candlestick.high;
    double low = lowest_candlestick.low;

    //step value for the y axis
    double step = (high-low)/23;

    //initialize array obj
    std::array<std::string, 24> strings;

    //output price title
    std::cout << "PRICE" << std::endl;

    //sets the y axis and precision of the step values
    for(int i = 0; i < strings.size(); i++){
        //std format, format string
        std::stringstream stream;
        stream << std::fixed << std::setprecision(precisionSize) << std::setw(5) << low + i * step;
        strings[i] = stream.str() + " -| ";
    }

    //make a graph for each candlestick!
    for(int i = 0; i < candlesticks.size(); i++){
        stringBuilder(step, low, candlesticks[i], strings);

    }

    //iterate back through to draw the strings
    for(int i = strings.size() - 1; i >= 0; i--){
        std::cout << strings[i] << std::endl;
    }

    std::cout<<"           ";
    for(int i = 0; i < candlesticks.size(); i++){
        std::cout << "...............";
    }
    std::cout<< "PRODUCT:  " << product << std::endl;

    std::cout<<"   ";
    for(int i = 0; i < candlesticks.size(); i++){
        std::string tempDate = candlesticks[i].date;
        std::vector<std::string> tokenDates = CSVReader::tokenise(tempDate, ' ');
        std::vector<std::string> tokenDates2 = CSVReader::tokenise(tokenDates[1], '.');

        std::cout<< "        "<< tokenDates2[0];
    }
    std::cout<<std::endl;
}
/**string builder for volume, takes a candlestick, step size, and array of strings**/
void CandleStickGraph::volumeBuilder(double step, CandleStick& candle, std::array<std::string, 8>& strings) {
    //sets the "top value" relative
    int top = int(candle.volume / step);
    //iterates until that "top" value
    //sets contents of bar graph
    for(int i = 0; i < top; i++){
        strings[i] += "     |||      ";
    }
    for(int i = top; i < 8; i++){
        //adding spacing to specified size
        strings[i] += "              ";
    }
}
/**actual construction of price volume bar graph, takes a vector of candlesticks and a string of product types**/
void CandleStickGraph::volumeGraph(std::vector<CandleStick> candlesticks, std::string productType){
    //set the array size 8
    std::array<std::string, 8> strings;
    CandleStick highest_candlestick = *std::max_element(candlesticks.begin(), candlesticks.end(), [](CandleStick a, CandleStick b){return a.volume < b.volume;});//find candlestick with highest volume to set the scale
    double highestVol = highest_candlestick.volume;

    //calculate step by the size of strings
    double step = highestVol / 7;
    highestVol += step;

    //set the product string
    std::string product;


    std::vector<std::string> tokens = CSVReader::tokenise(productType, ',');
    if (tokens.size() != 2)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << productType << std::endl;
    }
    else{
        //product name
        product = tokens[0];
    }

    //get the precision
    int precision = getPrecision(product);

    //sets the y axis and precision of the step values
    for(int i = 0; i < strings.size(); i++){
        //std format, format string
        std::stringstream stream;
        stream << std::fixed << std::setprecision(precision) << std::setw(10) << 0 + i * step;
        strings[i] = stream.str() + " -| ";
    }

    for(int i = 0; i < candlesticks.size(); i++){
        volumeBuilder(step, candlesticks[i], strings);
    }

    //iterate back through to draw the strings
    for(int i = strings.size() - 1; i >= 0; i--){
        std::cout << strings[i] << std::endl;
    }

    std::cout<<"           ";
    for(int i = 0; i < candlesticks.size(); i++){
        std::cout << "................";
    }
    std::cout<<std::endl;

    std::cout<<"            ";
    //output dates
    for(int i = 0; i < candlesticks.size(); i++){
        std::string tempDate = candlesticks[i].date;
        std::vector<std::string> tokenDates = CSVReader::tokenise(tempDate, ' ');
        std::vector<std::string> tokenDates2 = CSVReader::tokenise(tokenDates[1], '.');

        std::cout<< "      "<< tokenDates2[0];
    }
    std::cout<<std::endl;
}

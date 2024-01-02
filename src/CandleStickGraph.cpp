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
    //std::array<std::string, 8> strings;
    //strings.fill(""); // Initialize all elements with an empty string
    const std::string GREEN_COLOR = "\033[32m";//set the colorways
    const std::string RED_COLOR = "\033[31m";
    const std::string RESET_COLOR = "\033[0m";

    int openPos = int((candle.open - low)/step);//define each position relative to low and step
    int closePos = int((candle.close - low)/step);
    int highPos = int((candle.high - low)/step);
    int lowPos = int((candle.low - low)/ step);

    //std::string spacing = "     ";

    std::string color = (candle.close < candle.open) ? RED_COLOR : GREEN_COLOR;//determine coloring of the candlesticks


    for (int i = 0; i < lowPos; i++) {//handles starting blank space
        strings[i].append("              ");
    }
    for (int i = lowPos; i < std::min(openPos, closePos); ++i) {//keeps adding until at the top
        //std::cout << "it's happening" << std::endl;
        //strings[i] += "      |       " ;
        strings[i] += color + "      |       " + RESET_COLOR;
    }
    strings[std::min(openPos, closePos)] +=  "     ___      " ;//set the close value
    for (int i = std::min(openPos, closePos)+1; i < std::max(openPos, closePos); i++){//body of the candlestick
        //strings[i] += "| |      ";
        strings[i] += color + "     | |      " + RESET_COLOR;
    }
    if(openPos != closePos){
        //strings[std::max(openPos, closePos)] += "___      ";
        strings[std::max(openPos, closePos)] += color + "     ___      " + RESET_COLOR;//open value, if not at the same value as close
    }
    for (int i = std::max(openPos, closePos) + 1; i < highPos + 1; i++){
        //std::cout << "it's happening 2" << std::endl;
        //strings[i] += " |       ";
        strings[i] += color + "      |       " + RESET_COLOR;
    }
    for(int i = highPos + 1; i < 16; i++){
        strings[i] += "              ";
    }

}

int CandleStickGraph::getPrecision(std::string product){


    //the precision will change dependent on the product currency type
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


    std::string product;//set the product string


    std::vector<std::string> tokens = CSVReader::tokenise(productType, ',');
    if (tokens.size() != 2)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << productType << std::endl;
    }
    else{
        product = tokens[0]; //product name
    }



    int precisionSize = getPrecision(product);

    //long double step = (high-low)/15;

    double high = highest_candlestick.high;
    double low = lowest_candlestick.low;


    double step = (high-low)/23;//step value for the y axis

    std::array<std::string, 24> strings;//initialize array obj


    std::cout << "PRICE" << std::endl; //output price title

    for(int i = 0; i < strings.size(); i++){//sets the y axis and precision of the step values
        //std format, format string
        std::stringstream stream;
        stream << std::fixed << std::setprecision(precisionSize) << std::setw(5) << low + i * step;
        strings[i] = stream.str() + " -| ";
    }


    for(int i = 0; i < candlesticks.size(); i++){//make a graph for each candlestick!

        /*
        std::cout << "candlestick low val" << candlesticks[i].low << std::endl;
        std::cout << "candlestick high val" << candlesticks[i].high << std::endl;
        std::cout << "candlestick open val" << candlesticks[i].open << std::endl;
        std::cout << "candlestick close val" << candlesticks[i].close << std::endl;
        std::cout << "candlestick timestamp" << candlesticks[i].date << std::endl;
        std::cout << "=====================" << std::endl;
        */

        stringBuilder(step, low, candlesticks[i], strings);

    }
    for(int i = strings.size() - 1; i >= 0; i--){//iterate back through to draw the strings
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
    int top = int(candle.volume / step);//sets the "top value" relative
    for(int i = 0; i < top; i++){//iterates until that "top" value
        strings[i] += "     ***      ";//sets contents of bar graph
    }
    for(int i = top; i < 8; i++){
        strings[i] += "              ";//adding spacing to specified size
    }
}
/**actual construction of price volume bar graph, takes a vector of candlesticks and a string of product types**/
void CandleStickGraph::volumeGraph(std::vector<CandleStick> candlesticks, std::string productType){
    std::array<std::string, 8> strings;//set the array size 8
    CandleStick highest_candlestick = *std::max_element(candlesticks.begin(), candlesticks.end(), [](CandleStick a, CandleStick b){return a.volume < b.volume;});//find candlestick with highest volume to set the scale
    double highestVol = highest_candlestick.volume;

    double step = highestVol / 7;//calculate step by the size of strings
    highestVol += step;

    std::string product;//set the product string


    std::vector<std::string> tokens = CSVReader::tokenise(productType, ',');
    if (tokens.size() != 2)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << productType << std::endl;
    }
    else{
        product = tokens[0]; //product name
    }

    int precision = getPrecision(product);//get the precision

    for(int i = 0; i < strings.size(); i++){//sets the y axis and precision of the step values
        //std format, format string
        std::stringstream stream;
        stream << std::fixed << std::setprecision(precision) << std::setw(10) << 0 + i * step;
        strings[i] = stream.str() + " -| ";
    }

    for(int i = 0; i < candlesticks.size(); i++){
        volumeBuilder(step, candlesticks[i], strings);
    }

    for(int i = strings.size() - 1; i >= 0; i--){//iterate back through to draw the strings
        std::cout << strings[i] << std::endl;
    }

    std::cout<<"           ";
    for(int i = 0; i < candlesticks.size(); i++){
        std::cout << "................";
    }
    std::cout<<std::endl;

    std::cout<<"            ";//output dates
    for(int i = 0; i < candlesticks.size(); i++){
        std::string tempDate = candlesticks[i].date;
        std::vector<std::string> tokenDates = CSVReader::tokenise(tempDate, ' ');
        std::vector<std::string> tokenDates2 = CSVReader::tokenise(tokenDates[1], '.');

        std::cout<< "      "<< tokenDates2[0];
    }
    std::cout<<std::endl;
}

#include "CandleStick.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "OrderBook.h"
#include <vector>
#include <iostream>
#include <string>

/**alternative string to type conversion**/
OrderBookType convert(const std::string& str)
{
    if(str == "bid") return OrderBookType::bid;
    else if(str == "ask") return OrderBookType::ask;
    else if(str == "asksale") return OrderBookType::asksale;
    else if(str == "bidsale") return OrderBookType::bidsale;
    else return OrderBookType::unknown;
}
/**epoch utility for if we want to be able to quantify time and sort things in a robust way apart from data being sorted, returns int of time**/
int convertToEpoch(const std::string& timestamp){
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
std::vector<CandleStick> CandleStick::computeCandlesticks(OrderBook& orderBook, const std::string& productType) {
    std::vector<CandleStick> candlesticks;
    std::string product;
    OrderBookType type;
    //first, tokenize the product type request argument into ask/bid (bookType), and product
    std::vector<std::string> tokens = CSVReader::tokenise(productType, ',');
    if (tokens.size() != 2)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << productType << std::endl;
    }
    else{
        product = tokens[0]; //product name
        type = convert(tokens[1]); //type name
    }

    //OrderBook orders = orderBook;

    //std::string startTime = orderBook.getEarliestTime();   //next isolate the timestamp of the first orderBook and store in a variable
    //int secondsPerCandlestick = 1;


    OrderBook currentTimeBlockOrders;

   //calculate the first candlestick as a base???


   double runningTotal = 0;
   double counter = 0;
   double open = 0;
   double close = 0;
   double low = 0;
   double high = 0;
   double isFirst = true;

   double volume = 0;

   OrderBookEntry intlOrder = orderBook.getOrderByIndex(0);

   //int i = 0;
   //start looping
   //while(true){
   for(int i = 0; i < orderBook.getOrderSetSize(); i++){
       OrderBookEntry order = orderBook.getOrderByIndex(i);

       //int timeInEpoch = convertToEpoch(order.timestamp);
       //int startTimeInEpoch = convertToEpoch(startTime);


       //if(timeInEpoch > startTimeInEpoch + secondsPerCandlestick){
       if(OrderBookEntry::compareByTimestamp(intlOrder, order)){//time is up!!
           //compute the candlestick!
           //open????

           //if this is the first candlestick being created
           if(isFirst){

               close = runningTotal/counter;
               open = close;
               //low = currentTimeBlockOrders.getLowPrice();
               //high = currentTimeBlockOrders.getHighPrice();

               //set the values and ignore it
               isFirst = false;//set flag to false
           }
           else{
               close = runningTotal/counter;
               low = currentTimeBlockOrders.getLowPrice();
               high = currentTimeBlockOrders.getHighPrice();

               //push new candlestick object to candlestick
               candlesticks.emplace_back(CandleStick(order.timestamp, open, high, low, close, volume));

               //shift the startTime
               //startTime = order.timestamp; //current order at this index is now the start
               intlOrder = order;
               //reset the counter
               counter = 0;
               volume = 0;
               runningTotal = 0;
               open = close;
               currentTimeBlockOrders.removeAllElements(); //flush the timeblock
           }


       } else{//still within timeframe

           //filter out for type of things to "add"
           if(order.orderType == type && order.product == product){

               //increase the running total
               runningTotal += (order.price * order.amount);
               //increase volume
               volume += order.price;
               //increase the counter
               counter += order.amount;
               currentTimeBlockOrders.insertOrder(order);//add current order to block of orders for the candlestick
           }
           //i++;
       }
   }
   return candlesticks;

}



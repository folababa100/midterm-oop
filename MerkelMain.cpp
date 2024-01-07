#include "MerkelMain.h"
#include <iostream>
#include <vector>
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "Wallet.h"
#include "CandleStick.h"
#include "CandleStickGraph.h"

MerkelMain::MerkelMain()
{

}

void MerkelMain::init()
{
    int input;
    currentTime = orderBook.getEarliestTime();

    wallet.insertCurrency("BTC", 10);

    while(true)
    {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }
}


void MerkelMain::printMenu()
{
    std::cout << "1: Print help " << std::endl;
    std::cout << "2: Print exchange stats" << std::endl;
    std::cout << "3: Make an offer (sell) " << std::endl;
    std::cout << "4: Make a bid (buy) " << std::endl;
    std::cout << "5: Print wallet " << std::endl;
    std::cout << "6: Create a text-based plot" << std::endl;
    std::cout << "7: Plot a text graph" << std::endl;
    std::cout << "8: Go to the next" << std::endl;
    std::cout << "============== " << std::endl;
    std::cout << "Current time is: " << currentTime << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}

void MerkelMain::printMarketStats()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        // Color codes from https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
        std::cout << "\033[32m" << "Product: " << "\033[0m" << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask,
                                                                p, currentTime);
        std::cout << "\033[33m" << "Asks seen: " << "\033[0m" << entries.size() << std::endl;
        std::cout << "\033[31m" << "Max ask: " << "\033[0m" << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "\033[34m" << "Min ask: " << "\033[0m" << OrderBook::getLowPrice(entries) << std::endl;
    }
}

void MerkelMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2],
                currentTime,
                tokens[0],
                OrderBookType::ask
            );
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterAsk Bad input " << std::endl;
        }
    }
}

void MerkelMain::enterBid()
{
    std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2],
                currentTime,
                tokens[0],
                OrderBookType::bid
            );
            obe.username = "simuser";

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }
    }
}

void MerkelMain::generateWallet()
{
    std::cout << wallet.toString() << std::endl;
}

void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl;
            if (sale.username == "simuser")
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }

    }

    currentTime = orderBook.getNextTime(currentTime);
}

int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in 1-8" << std::endl;
    std::getline(std::cin, line);
    try{
        userOption = std::stoi(line);
    }catch(const std::exception& e)
    {
        //
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

// I wrote the function below
void MerkelMain::generateCandleSticks(){

    std::string input = "";

    std::cout << "Please enter 'product,ask/bid' to see a candlestick graph: " << std::endl;
    //accept user input
    std::getline (std::cin,input);
    //get candlesticks
    std::vector<CandleStick> candlesticks = CandleStick::computeCandlesticks(orderBook, input);

    //returns a random set
    std::vector<CandleStick> candlesticksGroup(candlesticks.begin() + 1, candlesticks.begin() + 11);
    CandleStickGraph graph;
    //builds out a new graph
    graph.buildCandlestick(candlesticksGroup, input);


}
// I wrote the function below
void MerkelMain::generateVolume(){
    std::string input = "";
    std::cout << "Please enter 'product,ask/bid' to see a 'price volume' graph:" << std::endl;

    //accept user input
    std::getline (std::cin,input);
    //get candlesticks
    std::vector<CandleStick> candlesticks = CandleStick::computeCandlesticks(orderBook, input);
    //returns a random set
    std::vector<CandleStick> candlesticksGroup(candlesticks.begin() + 1, candlesticks.begin() + 11);
    CandleStickGraph graph;
    //builds out a new graph
    graph.volumeGraph(candlesticksGroup, input);
}

// I refactored the function below from if cases to switch cases
void MerkelMain::processUserOption(int userOption)
{
    switch (userOption) {
        case 0: // bad input
            std::cout << "Invalid choice. Choose 1-8" << std::endl;
            break;
        case 1:
            printHelp();
            break;
        case 2:
            printMarketStats();
            break;
        case 3:
            enterAsk();
            break;
        case 4:
            enterBid();
            break;
        case 5:
            generateWallet();
            break;
        case 6:
            generateCandleSticks();
            break;
        case 7:
            generateVolume();
            break;
        case 8:
            gotoNextTimeframe();
            break;
        default:
            std::cout << "Invalid choice. Choose 1-8" << std::endl;
            break;
    }
}

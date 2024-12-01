#include "include/orderBook.h" 
#include "include/trader.h"

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>

using namespace StockTradingSystem;

std::mutex orderBookMutex;

int getRandomInt(int min, int max) {
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

// Utility function to generate random stock price
double getRandomPrice(double min, double max) {
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> dist(min, max);
    return dist(rng);
}

// Define a trader function to simulate traders placing orders
void traderFunction(Trader& trader, OrderBook& orderBook) {
    // Randomize stocks with different prices and symbols for each trader
    Stock stockA(getRandomPrice(100, 200), "StockA_" + std::to_string(getRandomInt(1, 100)));
    Stock stockB(getRandomPrice(200, 300), "StockB_" + std::to_string(getRandomInt(1, 100)));
    
    trader.addStock(stockA);
    trader.addStock(stockB);

    // Randomize quantities for buy and sell orders
    int buyQuantity = getRandomInt(10, 50);
    int sellQuantity = getRandomInt(10, 50);

    // Create buy and sell orders with dynamic quantities and prices
    std::shared_ptr<Order> buyOrder = std::make_shared<MarketOrder>(trader.getID(), stockA.getPrice(), buyQuantity);
    buyOrder->setOrderType(OrderType::BUY);

    std::shared_ptr<Order> sellOrder = std::make_shared<MarketOrder>(trader.getID(), stockB.getPrice(), sellQuantity);
    sellOrder->setOrderType(OrderType::SELL);

    // Add the orders to the order book
    std::lock_guard<std::mutex> lock(orderBookMutex);
    orderBook.addOrder(buyOrder);
    orderBook.addOrder(sellOrder);

    // Randomized trading thresholds and actions
    double buyThreshold = getRandomPrice(150, 180);
    double sellThreshold = getRandomPrice(220, 260);
    
    trader.trade(buyThreshold, sellThreshold, buyQuantity, sellQuantity);
}

int main() {
    // Create an order book
    OrderBook orderBook;

    // Create and start multiple trader threads
    std::vector<std::thread> traderThreads;
    int numTraders = 3;
    for (int i = 0; i < numTraders; ++i) {
        Trader trader("Trader " + std::to_string(i + 1));
        traderThreads.emplace_back(traderFunction, std::ref(trader), std::ref(orderBook));
    }

    // Join trader threads
    for (auto& thread : traderThreads) {
        thread.join();
    }

    // Print the order book after all trading is done
    orderBook.printOrderBook();

    return 0;
}

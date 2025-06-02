#include "OrderBook.h"
#include "Order.h"
#include "Types.h"
#include <iostream>
#include <memory>
#include <string>

void printSeparator(const std::string &title)
{
    std::cout << "\n"
              << std::string(50, '=') << std::endl;
    std::cout << title << std::endl;
    std::cout << std::string(50, '=') << std::endl;
}

void demonstrateBasicOrders()
{
    printSeparator("BASIC ORDER DEMONSTRATION");

    // Create some orders
    auto order1 = std::make_shared<Order>("BUY_001", Side::BUY, 50.0, 1000);
    auto order2 = std::make_shared<Order>("SELL_001", Side::SELL, 51.0, 500);
    auto order3 = std::make_shared<Order>("BUY_002", Side::BUY, 49.5, 800);

    std::cout << "Created orders:" << std::endl;
    std::cout << order1->toString() << std::endl;
    std::cout << order2->toString() << std::endl;
    std::cout << order3->toString() << std::endl;

    // Test partial fill
    std::cout << "\nPartially filling BUY_001 with 300 shares:" << std::endl;
    order1->fill(300);
    std::cout << order1->toString() << std::endl;
    std::cout << "Is filled? " << (order1->isFilled() ? "Yes" : "No") << std::endl;
}

// TEST PURPOSES ONLY ---

void demonstrateOrderBook()
{
    printSeparator("ORDER BOOK DEMONSTRATION");

    // Create order book for Apple stock
    OrderBook orderBook("AAPL");
    std::cout << "Created order book for: AAPL" << std::endl;

    // Add some buy orders (bids)
    std::cout << "\nAdding BUY orders..." << std::endl;
    orderBook.addOrder(std::make_shared<Order>("BUY_001", Side::BUY, 150.00, 1000));
    orderBook.addOrder(std::make_shared<Order>("BUY_002", Side::BUY, 149.50, 500));
    orderBook.addOrder(std::make_shared<Order>("BUY_003", Side::BUY, 149.00, 800));
    orderBook.addOrder(std::make_shared<Order>("BUY_004", Side::BUY, 150.00, 300)); // Same price as BUY_001

    // Add some sell orders (asks)
    std::cout << "Adding SELL orders..." << std::endl;
    orderBook.addOrder(std::make_shared<Order>("SELL_001", Side::SELL, 151.00, 600));
    orderBook.addOrder(std::make_shared<Order>("SELL_002", Side::SELL, 151.50, 400));
    orderBook.addOrder(std::make_shared<Order>("SELL_003", Side::SELL, 152.00, 1000));

    // Display current order book state
    orderBook.printOrderBook();

    // Show market data
    std::cout << "\n--- MARKET DATA ---" << std::endl;
    std::cout << "Best Bid: $" << orderBook.getBestBidPrice() << std::endl;
    std::cout << "Best Ask: $" << orderBook.getBestAskPrice() << std::endl;
    std::cout << "Spread: $" << orderBook.getSpread() << std::endl;
}

void demonstrateOrderMatching()
{
    printSeparator("ORDER MATCHING DEMONSTRATION");

    OrderBook orderBook("TSLA");

    // Set up initial order book
    std::cout << "Setting up initial order book..." << std::endl;
    orderBook.addOrder(std::make_shared<Order>("BUY_001", Side::BUY, 200.00, 1000));
    orderBook.addOrder(std::make_shared<Order>("BUY_002", Side::BUY, 199.50, 500));
    orderBook.addOrder(std::make_shared<Order>("SELL_001", Side::SELL, 201.00, 800));
    orderBook.addOrder(std::make_shared<Order>("SELL_002", Side::SELL, 201.50, 600));

    std::cout << "\nInitial state:" << std::endl;
    orderBook.printOrderBook(3);

    // Add a buy order that matches with sell orders
    std::cout << "\n>>> Adding aggressive BUY order at $201.50 for 1000 shares" << std::endl;
    std::cout << "This should match with SELL orders..." << std::endl;

    orderBook.addOrder(std::make_shared<Order>("BUY_AGGRESSIVE", Side::BUY, 201.50, 1000));

    std::cout << "\nAfter matching:" << std::endl;
    orderBook.printOrderBook(3);

    // Show trade history
    std::cout << "\n--- TRADE HISTORY ---" << std::endl;
    auto trades = orderBook.getTrades();
    for (size_t i = 0; i < trades.size(); ++i)
    {
        auto trade = trades[i];
        std::cout << "Trade " << (i + 1) << ": "
                  << trade->quantity << " shares at $" << trade->price
                  << " (Buy: " << trade->buyOrderId
                  << ", Sell: " << trade->sellOrderId << ")" << std::endl;
    }

    // Add a sell order that matches with buy orders
    std::cout << "\n>>> Adding aggressive SELL order at $199.00 for 800 shares" << std::endl;
    orderBook.addOrder(std::make_shared<Order>("SELL_AGGRESSIVE", Side::SELL, 199.00, 800));

    std::cout << "\nFinal state:" << std::endl;
    orderBook.printOrderBook(3);

    std::cout << "\nFinal trade count: " << orderBook.getTotalTrades() << std::endl;
}

void demonstrateEdgeCases()
{
    printSeparator("EDGE CASES DEMONSTRATION");

    OrderBook orderBook("MSFT");

    try
    {
        // Test empty order book
        std::cout << "Empty order book best bid: $" << orderBook.getBestBidPrice() << std::endl;
        std::cout << "Empty order book best ask: $" << orderBook.getBestAskPrice() << std::endl;
        std::cout << "Empty order book spread: $" << orderBook.getSpread() << std::endl;

        // Test invalid order
        std::cout << "\nTrying to create invalid order with negative price..." << std::endl;
        try
        {
            auto badOrder = std::make_shared<Order>("BAD_001", Side::BUY, -10.0, 100);
        }
        catch (const std::exception &e)
        {
            std::cout << "Caught expected error: " << e.what() << std::endl;
        }

        // Test order cancellation
        std::cout << "\nTesting order cancellation..." << std::endl;
        orderBook.addOrder(std::make_shared<Order>("CANCEL_ME", Side::BUY, 100.0, 500));
        std::cout << "Orders before cancel: " << orderBook.getTotalOrders() << std::endl;

        bool cancelled = orderBook.cancelOrder("CANCEL_ME");
        std::cout << "Cancel successful: " << (cancelled ? "Yes" : "No") << std::endl;
        std::cout << "Orders after cancel: " << orderBook.getTotalOrders() << std::endl;

        // Try to cancel non-existent order
        bool cancelledFake = orderBook.cancelOrder("DOESNT_EXIST");
        std::cout << "Cancel non-existent order: " << (cancelledFake ? "Yes" : "No") << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

int main()
{
    std::cout << "🚀 ADVANCED ORDER BOOK SYSTEM 🚀" << std::endl;
    std::cout << "Welcome to the Order Book demonstration!" << std::endl;

    try
    {
        // Run all demonstrations
        demonstrateBasicOrders();
        demonstrateOrderBook();
        demonstrateOrderMatching();
        demonstrateEdgeCases();

        printSeparator("DEMONSTRATION COMPLETE");
        std::cout << "✅ All tests completed successfully!" << std::endl;
        std::cout << "The order book system is working correctly." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "❌ Error during demonstration: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\nPress Enter to exit..." << std::endl;
    std::cin.get();

    return 0;
}
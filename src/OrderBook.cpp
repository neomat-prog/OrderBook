#include "OrderBook.h"
#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include <iostream>

OrderBook::OrderBook(const std::string &symbol) : m_symbol(symbol)
{
    if (symbol.empty())
    {
        throw std::invalid_argument("Symbol cannot be empty");
    }
}

void OrderBook::addOrder(OrderPtr order)
{
    validateOrder(order);

    // Check if order already exists
    if (m_orders.find(order->getOrderId()) != m_orders.end())
    {
        throw std::invalid_argument("Order with ID " + order->getOrderId() + "already exists");
    }

    // Add to tracking
    m_orders[order->getOrderId()] = order;

    // Try to match the order
    matchOrder(order);

    // If order still has remaining quantity, add to appropriate level
    if (!order->isFilled())
    {
        addToAppropriateLevel(order);
    }
}

void OrderBook::matchOrder(OrderPtr order)
{
    if (order->getSide() == Side::BUY)
    {
        matchBuyOrder(order);
    }
    else
    {
        matchSellOrder(order);
    }
}

void OrderBook::matchBuyOrder(OrderPtr buyOrder)
{
    // Match against ask levels ( sell orders )
    // We want to match with the lowest ask prices first

    auto askIt = m_askLevels.begin(); // Start with the lowest price

    while (askIt != m_askLevels.end() && !buyOrder->isFilled())
    {
        Price askPrice = askIt->first;
        auto askLevel = askIt->second;

        // Can only match if buy price >= ask price
        if (buyOrder->getPrice() < askPrice)
        {
            break; // No more matches possible ( prices sorted )
        }

        // Match as much as possible at this price level
        std::vector<OrderPtr> filledOrders;
        Quantity quantityMatched = askLevel->match(
            buyOrder->getRemainingQuantity(),
            filledOrders);

        for (auto sellOrder : filledOrders)
        {
            Quantity tradeQuantity = std::min(quantityMatched, sellOrder->getQuantity());
            recordTrade(buyOrder, sellOrder, askPrice, tradeQuantity);
        }

        // Fill the buy order
        buyOrder->fill(quantityMatched);

        // Remove empty price level
        if (askLevel->isEmpty())
        {
            askIt = m_askLevels.erase(askIt);
        }
        else
        {
            askIt++;
        }
    }
}

void OrderBook::matchSellOrder(OrderPtr sellOrder)
{
    // Match agains bid levels ( buy orders )
    // We want to match with the HIGHEST bid prices first

    auto bidIt = m_bidLevels.rbegin(); // Start with highest bid price ( reverse iterator )

    while (bidIt != m_bidLevels.rend() && !sellOrder->isFilled())
    {
        Price bidPrice = bidIt->first;
        auto bidLevel = bidIt->second;

        // Can only match if sell price <= bid price
        if (sellOrder->getPrice() > bidPrice)
        {
            break;
        }

        // Match as much as possible at this price level
        std::vector<OrderPtr> filledOrders;
        Quantity quantityMatched = bidLevel->match(
            sellOrder->getRemainingQuantity(),
            filledOrders);

        for (auto buyOrder : filledOrders)
        {
            Quantity tradeQuantity = std::min(quantityMatched, buyOrder->getQuantity());
            recordTrade(buyOrder, sellOrder, bidPrice, tradeQuantity);
        }

        // Fill the sell order
        sellOrder->fill(quantityMatched);

        // Remove empty price level and advance iterator properly
        if (bidLevel->isEmpty())
        {
            // covert reverse iterator to forward iterator for erase
            auto forwardIt = std::next(bidIt).base();
            m_bidLevels.erase(forwardIt);
            bidIt = m_bidLevels.rbegin();
        }
        else
        {
            bidIt++;
        }
    }
}

void OrderBook::addToAppropriateLevel(OrderPtr order)
{
    Price price = order->getPrice();
    Side side = order->getSide();

    if (side == Side::BUY)
    {
        // Add to bid levels
        if (m_bidLevels.find(price) == m_bidLevels.end())
        {
            m_bidLevels[price] = std::make_shared<PriceLevel>(price);
        }
        m_bidLevels[price]->addOrder(order);
    }
    else
    {
        // Add to ask levels
        if (m_askLevels.find(price) == m_askLevels.end())
        {
            m_askLevels[price] = std::make_shared<PriceLevel>(price);
        }
        m_askLevels[price]->addOrder(order);
    }
}

bool OrderBook::cancelOrder(const OrderId &orderId)
{
    auto it = m_orders.find(orderId);
    if (it == m_orders.end())
    {
        return false;
    }
    OrderPtr order = it->second;

    // todo: remove from price level ( we'd need to modify PriceLevel )

    order->fill(order->getRemainingQuantity());

    // remove from tracking
    m_orders.erase(it);

    return true;
}

void OrderBook::recordTrade(OrderPtr buyOrder, OrderPtr sellOrder, Price price, Quantity quantity)
{
    auto trade = std::make_shared<Trade>(
        buyOrder->getOrderId(),
        sellOrder->getOrderId(),
        price,
        quantity);
    m_trades.push_back(trade);
}

Price OrderBook::getBestBidPrice() const
{
    if (m_bidLevels.empty())
    {
        return 0.0;
    }
    return m_bidLevels.rbegin()->first;
}

Price OrderBook::getBestAskPrice() const
{
    if (m_askLevels.empty())
    {
        return 0.0;
    }
    return m_askLevels.begin()->first;
}

Price OrderBook::getSpread() const
{
    Price bestBid = getBestBidPrice();
    Price bestAsk = getBestAskPrice();

    if (bestBid == 0.0 || bestAsk == 0.0)
    {
        return 0.0;
    }

    return bestAsk - bestBid;
}

void OrderBook::validateOrder(OrderPtr order) const
{
    if (!order)
    {
        throw std::invalid_argument("Order cannot be null");
    }

    if (order->isFilled())
    {
        throw std::invalid_argument("Cannot add already filled order");
    }
}

void OrderBook::printOrderBook(int levels) const
{
    std::cout << "\n=== ORDER BOOK FOR " << m_symbol << " ===" << std::endl;
    std::cout << std::fixed << std::setprecision(2);

    // Print ask levels (top to bottom, highest to lowest price)
    std::cout << "\nASKS (Sellers):" << std::endl;
    std::cout << "Price    | Quantity | Orders" << std::endl;
    std::cout << "---------|----------|-------" << std::endl;

    auto askIt = m_askLevels.rbegin();
    int askCount = 0;
    while (askIt != m_askLevels.rend() && askCount < levels)
    {
        std::cout << "$" << std::setw(7) << askIt->first
                  << " | " << std::setw(8) << askIt->second->getTotalQuantity()
                  << " | " << askIt->second->getOrderCount() << std::endl;
        ++askIt;
        ++askCount;
    }

    // Print spread
    std::cout << "\n--- SPREAD: $" << getSpread() << " ---" << std::endl;

    // Print bid levels (top to bottom, highest to lowest price)
    std::cout << "\nBIDS (Buyers):" << std::endl;
    std::cout << "Price    | Quantity | Orders" << std::endl;
    std::cout << "---------|----------|-------" << std::endl;

    auto bidIt = m_bidLevels.rbegin();
    int bidCount = 0;
    while (bidIt != m_bidLevels.rend() && bidCount < levels)
    {
        std::cout << "$" << std::setw(7) << bidIt->first
                  << " | " << std::setw(8) << bidIt->second->getTotalQuantity()
                  << " | " << bidIt->second->getOrderCount() << std::endl;
        ++bidIt;
        ++bidCount;
    }

    std::cout << "\nTotal Orders: " << m_orders.size() << std::endl;
    std::cout << "Total Trades: " << m_trades.size() << std::endl;
}
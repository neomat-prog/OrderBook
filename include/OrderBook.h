#pragma once
#include "PriceLevel.h"
#include "Order.h"
#include <map>
#include <memory>
#include <vector>
#include <unordered_map>

// Trade represents a completed transaction

struct Trade
{
	OrderId buyOrderId;
	OrderId sellOrderId;
	Price price;
	Quantity quantity;
	std::chrono::system_clock::time_point timestamp;

	Trade(const OrderId &buyId, const OrderId &sellId, Price p, Quantity q)
		: buyOrderId(buyId), sellOrderId(sellId), price(p), quantity(q), timestamp(std::chrono::system_clock::now()) {}
};

using TradePtr = std::shared_ptr<Trade>;

class OrderBook
{
private:
	// Price level storage

	std::map<Price, std::shared_ptr<PriceLevel>> m_bidLevels; // Buy orders ( h to l )
	std::map<Price, std::shared_ptr<PriceLevel>> m_askLevels; // Sell orders( l to h )

	// Order tracking
	std::unordered_map<OrderId, OrderPtr> m_orders;

	// Trade history
	std::vector<TradePtr> m_trades;

	std::string m_symbol;

public:
	explicit OrderBook(const std::string &symbol);

	// Order management
	void addOrder(OrderPtr order);
	bool cancelOrder(const OrderId &orderId);
	OrderPtr getOrder(const OrderId &orderId) const;

	// Market data queries
	Price getBestBidPrice() const;
	Price getBestAskPrice() const;
	Quantity getBidQuantityAtPrice(Price price) const;
	Quantity getAskQuantityAtPrice(Price price) const;
	Price getSpread() const;

	// Order book state
	bool isEmpty() const;
	size_t getTotalOrders() const { return m_orders.size(); }
	size_t getTotalTrades() const { return m_trades.size(); }

	// Trade history
	const std::vector<TradePtr> &getTrades() const { return m_trades; }
	TradePtr getLastTrade() const;

	std::string toString() const;
	void printOrderBook(int levels = 5) const;

private:
	// Core matching logic
	void matchOrder(OrderPtr order);
	void matchBuyOrder(OrderPtr buyOrder);
	void matchSellOrder(OrderPtr sellOrder);

	// helper methods

	void addToAppropriateLevel(OrderPtr order);
	void removePriceLevelIfEmpty(Side side, Price price);
	void recordTrade(OrderPtr buyOrder, OrderPtr sellOrder, Price price, Quantity quantity);

	// Validation
	void validateOrder(OrderPtr order) const;
};

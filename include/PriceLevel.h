#pragma once
#include "Order.h"
#include <queue>
#include <vector>


// The price for this level
// FIFO queue of orders
// Sum of all order quantities

class PriceLevel {
private:
	Price m_price;
	std::queue<OrderPtr> m_orders;
	Quantity m_totalQuantity;
public:
	explicit PriceLevel(Price price);

	void addOrder(OrderPtr order);
	OrderPtr getNextOrder();
	void removeOrder();

	// Query methods
	Price getPrice() const { return m_price; }
	Quantity getTotalQuantity() const { return m_totalQuantity; }
	bool isEmpty() const { return m_orders.empty(); }
	size_t getOrderCount() const { return m_orders.size(); }

	// Matching operations
	Quantity match(Quantity quantity, std::vector<OrderPtr>& filledOrders);

	std::string toString() const;

private:
	void updateTotalQuantity();

};
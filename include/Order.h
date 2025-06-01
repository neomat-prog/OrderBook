#pragma once
#include "Types.h"
#include <memory>

class Order
{
private:
	OrderId m_orderId;
	Side m_side;
	Price m_price;
	Quantity m_quantity;
	Quantity m_remainingQuantity;
	Timestamp m_timestamp;

public:
	// Constructor
	Order(const OrderId &orderId, Side side, Price price, Quantity quantity);

	// Getters
	const OrderId &getOrderId() const { return m_orderId; }
	Side getSide() const { return m_side; }
	Price getPrice() const { return m_price; }
	Quantity getQuantity() const { return m_quantity; }
	Quantity getRemainingQuantity() const { return m_remainingQuantity; }
	const Timestamp &getTimestamp() const { return m_timestamp; }

	// Order operations

	void fill(Quantity quantity);
	bool isFilled() const { return m_remainingQuantity == 0; }

	// Display
	std::string toString() const;
};

using OrderPtr = std::shared_ptr<Order>;
#include "Order.h"
#include <sstream>
#include <stdexcept>

Order::Order(const OrderId& orderId, Side side, Price price, Quantity quantity)
	: m_orderId(orderId)
	, m_side(side)
	, m_price(price)
	, m_quantity(quantity)
	, m_remainingQuantity(quantity)
	, m_timestamp(std::chrono::system_clock::now())
{
	// Validation
	if (price <= 0) {
		throw std::invalid_argument("Price must be positive");
	}
	if (quantity <= 0) {
		throw std::invalid_argument("Quantity must be positive");
	}
	if (orderId.empty()) {
		throw std::invalid_argument("Order ID cannot be empty");
	}
}

void Order::fill(Quantity quantity) {
	if (quantity > m_remainingQuantity) {
		throw std::invalid_argument("Cannot fill more than remaining quantity");
	}
	m_remainingQuantity -= quantity;
}

std::string Order::toString() const {
	std::ostringstream oss;
	oss << "Order[ID=" << m_orderId
		<< ", Side=" << sideToString(m_side)
		<< ", Price=" << m_price
		<< ", Qty=" << m_quantity
		<< ", Remaining=" << m_remainingQuantity << "]";
	return oss.str();
}
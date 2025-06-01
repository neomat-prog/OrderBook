#include "PriceLevel.h"
#include <stdexcept>
#include <sstream>

PriceLevel::PriceLevel(Price price)
    : m_price(price), m_totalQuantity(0)
{
    if (price <= 0) {
        throw std::invalid_argument("Price must be positive");
    }
}

void PriceLevel::addOrder(OrderPtr order) {
    // Validation
    if (!order) {
        throw std::invalid_argument("Order cannot be null");
    }
    
    if (std::abs(order->getPrice() - m_price) > 0.001) {  // ✅ Fixed: ord → order
        throw std::invalid_argument("Order price doesn't match price level");  // ✅ Fixed: added _argument
    }
    
    // Add to queue and update total
    m_orders.push(order);
    m_totalQuantity += order->getRemainingQuantity();
}

OrderPtr PriceLevel::getNextOrder() {
    if (m_orders.empty()) {
        return nullptr;
    }
    return m_orders.front();
}

void PriceLevel::removeOrder() {
    if (m_orders.empty()) {
        throw std::runtime_error("Cannot remove from empty price level");
    }
    
    // Update total before removing
    OrderPtr order = m_orders.front();
    m_totalQuantity -= order->getRemainingQuantity();
    m_orders.pop();
}

Quantity PriceLevel::match(Quantity requestedQuantity, std::vector<OrderPtr>& filledOrders) {
    Quantity totalMatched = 0;
    
    while (!m_orders.empty() && totalMatched < requestedQuantity) {
        OrderPtr currentOrder = getNextOrder();
        Quantity availableInOrder = currentOrder->getRemainingQuantity();
        Quantity neededQuantity = requestedQuantity - totalMatched;
        
        // Determine how much to fill from this order
        Quantity quantityToFill = std::min(availableInOrder, neededQuantity);
        
        // Fill the order
        currentOrder->fill(quantityToFill);
        totalMatched += quantityToFill;
        m_totalQuantity -= quantityToFill;  // ✅ Fixed: added semicolon
        
        // Track filled orders for reporting
        filledOrders.push_back(currentOrder);
        
        // Remove order if completely filled
        if (currentOrder->isFilled()) {
            m_orders.pop();
        }
    }
    
    return totalMatched;  // ✅ Fixed: added semicolon
}

void PriceLevel::updateTotalQuantity() {
    m_totalQuantity = 0;
    
    // We need a temporary queue to iterate (since std::queue doesn't have iterators)
    std::queue<OrderPtr> tempQueue = m_orders;
    
    while (!tempQueue.empty()) {
        m_totalQuantity += tempQueue.front()->getRemainingQuantity();
        tempQueue.pop();
    }
}

std::string PriceLevel::toString() const {
    std::ostringstream oss;
    oss << "PriceLevel[Price=" << m_price
        << ", Orders=" << m_orders.size()
        << ", TotalQty=" << m_totalQuantity << "]";
    return oss.str();
}
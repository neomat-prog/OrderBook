#pragma once
#include <string>
#include <chrono>

using OrderId = std::string;
using Price = double;
using Quantity = int;
using Timestamp = std::chrono::system_clock::time_point;

enum class Side {
	BUY,
	SELL
};

inline std::string sideToString(Side side) {
	return (side == Side::BUY) ? "BUY" : "SELL";
}
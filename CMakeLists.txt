cmake_minimum_required(VERSION 3.10)
project("AdvancedOrderBook")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(SOURCES
    src/main.cpp
    src/Order.cpp
    src/PriceLevel.cpp
    src/OrderBook.cpp
)


set(HEADERS
    include/Order.h
    include/PriceLevel.h
    include/OrderBook.h
    include/Types.h
)


add_executable(AdvancedOrderBook ${SOURCES} ${HEADERS})


target_include_directories(AdvancedOrderBook PRIVATE include)

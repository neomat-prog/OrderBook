cmake_minimum_required(VERSION 3.10)
project("AdvancedOrderBook")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add all source files
set(SOURCES
    src/main.cpp
    src/Order.cpp
    src/PriceLevel.cpp
    src/OrderBook.cpp
)

# Add all header files
set(HEADERS
    include/Order.h
    include/PriceLevel.h
    include/OrderBook.h
    include/Types.h
)

# Create the executable
add_executable(AdvancedOrderBook ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(AdvancedOrderBook PRIVATE include)
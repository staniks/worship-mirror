#include "catch.hpp"

#include <mau/memory/value_container.hpp>

using namespace mau;

int* create_69(int* value)
{
    *value = 69;
    return value;
}
void destroy_69(int* value)
{
    *value = 0;
}

TEST_CASE("value_container_t is initialized and destroyed", "[value_container_t]")
{
    int value;
    {
        value_container_t<int*, decltype(&destroy_69)> container{create_69(&value), destroy_69};
        REQUIRE(value == 69);
    }
    REQUIRE(value == 0);
}

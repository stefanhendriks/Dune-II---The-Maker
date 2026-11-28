#include <catch2/catch_test_macros.hpp>
#include "gameobjects/units/UnitOrderQueue.hpp"

static UnitOrder moveOrder(int goalCell)
{
    UnitOrder order;
    order.kind = eUnitOrderKind::MOVE;
    order.goalCell = goalCell;
    return order;
}

TEST_CASE("UnitOrderQueue - a new queue has no current order", "[units][orders]")
{
    UnitOrderQueue queue;
    REQUIRE(queue.empty());
    REQUIRE(queue.size() == 0);
    REQUIRE_FALSE(queue.current().has_value());
}

TEST_CASE("UnitOrderQueue - a given order becomes the current order", "[units][orders]")
{
    UnitOrderQueue queue;
    queue.replace(moveOrder(10));

    REQUIRE(queue.size() == 1);
    REQUIRE(queue.current().has_value());
    REQUIRE(queue.current()->kind == eUnitOrderKind::MOVE);
    REQUIRE(queue.current()->goalCell == 10);
}

TEST_CASE("UnitOrderQueue - a new order replaces the current order", "[units][orders]")
{
    UnitOrderQueue queue;
    queue.replace(moveOrder(10));

    UnitOrder attack;
    attack.kind = eUnitOrderKind::ATTACK;
    attack.goalCell = 20;
    attack.attackUnit = 3;
    queue.replace(attack);

    REQUIRE(queue.size() == 1);
    REQUIRE(queue.current()->kind == eUnitOrderKind::ATTACK);
    REQUIRE(queue.current()->goalCell == 20);
    REQUIRE(queue.current()->attackUnit == 3);
}

TEST_CASE("UnitOrderQueue - completing the current order empties a queue of one", "[units][orders]")
{
    UnitOrderQueue queue;
    queue.replace(moveOrder(10));
    queue.completeCurrent();

    REQUIRE(queue.empty());
    REQUIRE_FALSE(queue.current().has_value());
}

TEST_CASE("UnitOrderQueue - completing without an order does nothing", "[units][orders]")
{
    UnitOrderQueue queue;
    queue.completeCurrent();
    REQUIRE(queue.empty());
}

TEST_CASE("UnitOrderQueue - clear drops the current order", "[units][orders]")
{
    UnitOrderQueue queue;
    queue.replace(moveOrder(10));
    queue.clear();
    REQUIRE(queue.empty());
}

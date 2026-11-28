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
    REQUIRE(queue.current() == nullptr);
    REQUIRE(queue.currentAs<UnitOrder>() == nullptr);
}

TEST_CASE("UnitOrderQueue - a given order becomes the current order", "[units][orders]")
{
    UnitOrderQueue queue;
    queue.replace(moveOrder(10));

    REQUIRE(queue.size() == 1);
    REQUIRE(queue.current() != nullptr);
    REQUIRE(queue.currentAs<UnitOrder>() != nullptr);
    REQUIRE(queue.currentAs<UnitOrder>()->kind == eUnitOrderKind::MOVE);
    REQUIRE(queue.currentAs<UnitOrder>()->goalCell == 10);
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
    REQUIRE(queue.currentAs<UnitOrder>()->kind == eUnitOrderKind::ATTACK);
    REQUIRE(queue.currentAs<UnitOrder>()->goalCell == 20);
    REQUIRE(queue.currentAs<UnitOrder>()->attackUnit == 3);
}

TEST_CASE("UnitOrderQueue - completing the current order empties a queue of one", "[units][orders]")
{
    UnitOrderQueue queue;
    queue.replace(moveOrder(10));
    queue.completeCurrent();

    REQUIRE(queue.empty());
    REQUIRE(queue.current() == nullptr);
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

TEST_CASE("UnitOrderQueue - a sequence of orders is carried out front first", "[units][orders]")
{
    UnitOrderQueue queue;
    queue.replace({PickupUnitOrder{.unitId = 4}, DropUnitOrder{.cell = 30}, LeaveMapOrder{}});

    REQUIRE(queue.size() == 3);
    REQUIRE(queue.currentAs<PickupUnitOrder>() != nullptr);
    REQUIRE(queue.currentAs<PickupUnitOrder>()->unitId == 4);
    REQUIRE(queue.currentAs<DropUnitOrder>() == nullptr);

    queue.completeCurrent();
    REQUIRE(queue.currentAs<DropUnitOrder>() != nullptr);
    REQUIRE(queue.currentAs<DropUnitOrder>()->cell == 30);
    REQUIRE(queue.currentAs<DropUnitOrder>()->unitMoveToCell == -1);

    queue.completeCurrent();
    REQUIRE(queue.currentAs<LeaveMapOrder>() != nullptr);

    queue.completeCurrent();
    REQUIRE(queue.empty());
}

TEST_CASE("UnitOrderQueue - a sequence of orders replaces the current order", "[units][orders]")
{
    UnitOrderQueue queue;
    queue.replace(moveOrder(10));
    queue.replace({DropUnitOrder{.cell = 30, .unitMoveToCell = 40}, LeaveMapOrder{}});

    REQUIRE(queue.size() == 2);
    REQUIRE(queue.currentAs<UnitOrder>() == nullptr);
    REQUIRE(queue.currentAs<DropUnitOrder>()->unitMoveToCell == 40);
}

TEST_CASE("UnitOrderQueue - contains finds an order of a type anywhere in the queue", "[units][orders]")
{
    UnitOrderQueue queue;
    REQUIRE_FALSE(queue.contains<LeaveMapOrder>());

    queue.replace({PickupUnitOrder{.unitId = 4}, DropUnitOrder{.cell = 30}, LeaveMapOrder{}});
    REQUIRE(queue.contains<PickupUnitOrder>());
    REQUIRE(queue.contains<LeaveMapOrder>());
    REQUIRE_FALSE(queue.contains<UnitOrder>());

    queue.completeCurrent();
    REQUIRE_FALSE(queue.contains<PickupUnitOrder>());
    REQUIRE(queue.contains<LeaveMapOrder>());
}

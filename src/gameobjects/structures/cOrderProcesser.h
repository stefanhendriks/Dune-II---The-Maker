#pragma once

#include "sidebar/cBuildingListItem.h"

class cPlayer;

class cOrderProcesser {
private:
    static constexpr auto kMaxItemsToOrder = 8;

public:
    explicit cOrderProcesser(cPlayer *thePlayer);

    ~cOrderProcesser();

    void think();    // time based (per second)
    void updatePricesForStarport();

    void addOrder(cBuildingListItem *item);

    bool acceptsOrders();    // able to accept orders?
    void removeOrder(cBuildingListItem *item);

    void placeOrder();

    void playTMinusSound(int seconds);

    /**
     * Can place an order (call for Frigate) when anything has been ordered and the order hasn't been placed yet.
     * @return
     */
    bool canPlaceOrder() {
        return hasOrderedAnything() &&
               isOrderPlaced() == false;
    }

    bool isOrderPlaced() {
        return m_orderPlaced;
    }

    bool isFrigateSent() {
        return m_frigateSent;
    }

    bool hasOrderedAnything();

    bool hasFreeSlot() {
        return getFreeSlot() >= 0;
    }

    cBuildingListItem *getItemToDeploy();

    void markOrderAsDeployed(cBuildingListItem *item);

    void setOrderHasBeenProcessed();

    void sendFrigate();

    void clear();

protected:
    void removeAllItems();

    void removeItem(int slot);

    int getFreeSlot();

    int getSlotForItem(cBuildingListItem *item);

    int getRandomizedSecondsToWait();

private:
    cBuildingListItem *m_orderedItems[kMaxItemsToOrder];
    int m_pricePaidForItem[kMaxItemsToOrder];

    cPlayer *m_player;

    bool m_orderPlaced;

    // sent frigate to deliver order
    bool m_frigateSent;
    int m_unitIdOfFrigateSent;

    // when above zero, this will be substracted by one (by the think function).
    int m_secondsUntilArrival;

    // this is randomly set to 60 or more seconds. After the time runs out, new prices
    // will be set in the LIST_STARPORT
    int m_secondsUntilNewPricesWillBeCalculated;
};

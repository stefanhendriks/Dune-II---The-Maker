#include <player/cPlayerDifficultySettings.h>

/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2021 (c) code by Stefan Hendriks

  */


class cBullet
{

public:

    void init();
    void draw();
    int pos_x() const;
    int pos_y() const;
    int  draw_x();
    int  draw_y();
    void think();       // think (when dying, doing damage, etc)
    void think_move();  // thinking for movement

    bool bAlive;

    int iCell;          // cell of bullet - calculated in think_move() based on posX/posY
    int iType;          // type of bullet    
    
    int iOwnerUnit;     // unit who shoots
    int iOwnerStructure;// structure who shoots (rocket/normal turret for example)
    int iPlayer;        // what player shot this? (for damage control)

    // when set, the bullet will follow the unit, but only for a while
    int iHoming;
    int TIMER_homing;   // when timer set, > 0 means homing (follow unit until timer runs out)

    // absolute coordinates of bullet
    float posX;
    float posY;
    float targetX;
    float targetY;

    int iFrame;         // frame for rockets

    int TIMER_frame;

private:

    int getBulletBmpWidth() const;
    int getBulletBmpHeight() const;

    void die();

    void damageStructure(int idOfStructureAtCell, double factor);

    cPlayer * getPlayer() const;

    float getDamageToInflictToNonInfantry() const;

    s_Bullet gets_Bullet() const;

    bool isGroundBullet() const;

    void moveBulletTowardsGoal();

    int getRandomX() const;

    int getRandomY() const;

    void damageWall(int cell, double factor) const;

    bool isAtDestination() const;

    void arrivedAtDestinationLogic();

    void damageSandworm(int cell, double factor) const;

    void detonateSpiceBloom(int cell, double factor) const;

    bool damageGroundUnit(int cell, double factor) const;

    float getDamageToInflictToInfantry() const;

    float getDamageToInflictToUnit(cUnit &unitTakingDamage) const;

    bool canDamageAirUnits() const;

    bool damageAirUnit(int cell, double factor) const;

    void damageTerrain(int cell, double factor) const;

    bool isSonicWave() const;

    bool isInfantryBullet() const;

    cPlayerDifficultySettings *getDifficultySettings() const;

    bool canDamageGround() const;
};


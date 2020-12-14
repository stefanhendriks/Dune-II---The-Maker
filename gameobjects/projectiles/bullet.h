#include <player/cPlayerDifficultySettings.h>

/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

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

    int iCell;          // cell of bullet   
    int iType;          // type of bullet    
    
    int iOwnerUnit;     // unit who shoots
    int iOwnerStructure;// structure who shoots (rocket/normal turret for example)
    int iPlayer;        // what player shot this? (for damage control)
    

    // when set, the bullet will track down the bastard, but only for a while
    int iHoming;        // homing to unit...
    int TIMER_homing;   // when timer set, > 0 means homing

    // Movement
    int iGoalCell;      // the goal cell (goal of path)    
    float iOffsetX;       // X offset
    float iOffsetY;       // Y offset

    int iFrame;         // frame for rockets

    int TIMER_frame;

private:

    int getBulletBmpWidth() const;
    int getBulletBmpHeight() const;

    void die();

    void damageStructure(int idOfStructureAtCell);

    cPlayer * getPlayer() const;

    int getDamageToInflictToNonInfantry() const;

    s_Bullet gets_Bullet() const;

    bool isNonFlyingTerrainBullet() const;

    bool isTurretBullet() const;

    void moveBulletTowardsGoal();

    int getRandomX() const;

    int getRandomY() const;

    void damageWall(int cell) const;

    bool isAtGoalCell() const;

    void arrivedAtGoalLogic();

    void damageSandworm(int cell) const;

    void detonateSpiceBloom(int cell) const;

    void damageGroundUnit(int cell) const;

    int getDamageToInflictToInfantry() const;

    int getDamageToInflictToUnit(cUnit &unitTakingDamage) const;

    bool isDeviatorGas() const;

    bool isRocket() const;

    void damageAirUnit(int cell) const;

    void damageTerrain(int cell) const;

    bool isSonicWave() const;

    bool isInfantryBullet() const;

    cPlayerDifficultySettings *getDifficultySettings() const;
};


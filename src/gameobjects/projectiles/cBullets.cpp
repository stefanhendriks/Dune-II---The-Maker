#include "cBullets.h"
#include "include/sGameServices.h"

void cBullets::serviceInit(sGameServices *services)
{
    for (auto &bullet : m_values) {
        bullet.serviceInit(services);
    }
}

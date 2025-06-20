#include "cIx.h"

#include "definitions.h"

// Constructor
cIx::cIx()
{
    // other variables (class specific)
}

int cIx::getType() const
{
    return IX;
}

void cIx::thinkFast()
{
    cAbstractStructure::thinkFast();
}

// Specific Animation thinking (flag animation OR its deploy animation)
void cIx::think_animation()
{
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
}

void cIx::think_guard()
{

}

#include "Entity.hpp"

thor::EventSystem<Message, std::string> Entity::messageSystem;

Entity::Entity()
{
}


std::string getEventId(const Message *message)
{
    return message->id;
}

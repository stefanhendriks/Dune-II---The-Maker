#ifndef ENTITY_HPP
#define ENTITY_HPP
#include <Thor/Input.hpp>

struct Message{
    Message(const std::string& id):
        id(id)
    {

    }

    std::string id;
};



class Entity
{
public:
    Entity();

private:
    static thor::EventSystem<Message, std::string> messageSystem;
};

#endif // ENTITY_HPP

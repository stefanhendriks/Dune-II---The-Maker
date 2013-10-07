#ifndef ENTITY_HPP
#define ENTITY_HPP
#include <Thor/Input.hpp>

struct Message{
    Message(const std::string& id):
      id(id) {}

    virtual ~Message() {}

    std::string id;
};


std::string getEventId(const Message &message);



class Entity
{
public:
    Entity();

protected:
    static thor::EventSystem<Message, std::string> messageSystem;
};

#endif // ENTITY_HPP

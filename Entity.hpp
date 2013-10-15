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

typedef thor::EventSystem<Message, std::string> MessageSystem;

#endif // ENTITY_HPP

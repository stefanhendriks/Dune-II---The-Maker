#ifndef MESSAGES_HPP
#define MESSAGES_HPP
#include <Thor/Input.hpp>

const std::string MESSAGES_PREMOVE   = "premove";
const std::string MESSAGES_UNITMOVE  = "unitMove";

struct Message{
    Message(const std::string& id);

    virtual ~Message();

    std::string id;
};


std::string getEventId(const Message &message);

typedef thor::EventSystem<Message, std::string> MessageSystem;

#endif // ENTITY_HPP

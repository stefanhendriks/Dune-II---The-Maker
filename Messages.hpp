#ifndef MESSAGES_HPP
#define MESSAGES_HPP
#include <Thor/Input.hpp>

enum class Messages {
  premove,
  unitMove
};

struct Message{
    Message(Messages id);

    virtual ~Message();

    Messages id;
};


Messages getEventId(const Message &message);

typedef thor::EventSystem<Message, Messages> MessageSystem;

#endif // ENTITY_HPP

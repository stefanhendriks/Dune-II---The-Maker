#include "Messages.hpp"

Messages getEventId(const Message &message)
{
    return message.id;
}


Message::Message(Messages id):
  id(id)
{
}

Message::~Message()
{
}

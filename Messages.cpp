#include "Messages.hpp"

std::string getEventId(const Message &message)
{
    return message.id;
}


Message::Message(const std::string &id):
  id(id)
{
}

Message::~Message()
{
}

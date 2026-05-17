#pragma once

enum eNotificationType {
    NEUTRAL,    // generic message, in white
    PRIORITY,   // a more important message, yellow
    BAD,        // bad news, in red
    OTHER       // other kind of messages
};
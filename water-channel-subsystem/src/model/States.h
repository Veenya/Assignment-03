#ifndef __STATES__
#define __STATES__

enum class SystemState {
    AUTOMATIC,
    MANUAL_LOCAL,
    MANUAL_REMOTE
};

enum class ConnectivityState {
    CONNECTED,
    UNCONNECTED
};

#endif
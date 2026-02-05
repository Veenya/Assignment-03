#ifndef __STATES__
#define __STATES__

enum class SystemState { AUTOMATIC, 
                            MANUAL, 
                            UNCONNECTED, 
                            NOT_AVAILABLE, 
                            AUTO_CLOSED, 
                            AUTO_OPEN_50, 
                            AUTO_OPEN_100, 
                            INVALID_STATUS 
                        };

#endif
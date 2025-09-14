#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>


class MemoryModule {
public:
    // Constructor
    MemoryModule(uint8_t cs, uint8_t wp, uint8_t clk,
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

    // Init the memory interface
    void init();

    // Read memory by address
    int readByAddr(size_t addr, char * buffer);

    // Write memory by address (Backdoor only)
    void writeByAddr(const uint8_t * buf, size_t size);

    // Read memory by DID
    int readById(size_t identifier, char * buffer);

    // Write memory by DID (Backdoor only)
    void writeById(const uint8_t * buf, size_t identifier);

}



#endif

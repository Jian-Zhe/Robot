#pragma once

#include "crsf/crc8/crc8.h"
#include "crsf_protocol.h"
#include "serial/serialib.h"
#include <chrono>
#include <stdint.h>

enum eFailsafeAction { fsaNoPulses, fsaHold };

class CrsfSerial
{
public:
    // Packet timeout where buffer is flushed if no data is received in this time
    static const unsigned int CRSF_PACKET_TIMEOUT_MS = 100;
    static const unsigned int CRSF_FAILSAFE_STAGE1_MS = 300;

    CrsfSerial(serialib &port, uint32_t baud = CRSF_BAUDRATE);
    void begin(uint32_t baud = 0);
    void write(uint8_t b);
    void write(const uint8_t *buf, size_t len);
    void queuePacket(uint8_t type, const void *payload, uint8_t len);

    uint32_t getBaud() const { return _baud; };

    const crsfLinkStatistics_t *getLinkStatistics() const { return &_linkStatistics; }
    bool isLinkUp() const { return _linkIsUp; }

    // Event Handlers
    void (*onLinkUp)();
    void (*onLinkDown)();
    void (*onPacketLinkStatistics)(crsfLinkStatistics_t *ls);
    
protected:
    serialib &_port;
    
    virtual void processPacketIn(uint8_t len);

    bool _linkIsUp;
    uint32_t _lastReceive;

    uint8_t _rxBuf[CRSF_MAX_PACKET_SIZE];
    uint8_t _rxBufPos;

    uint64_t millis() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();
    }

    // Packet Handlers
    void packetLinkStatistics(const crsf_header_t *p);

    void handleSerialIn();

private:
    Crc8 _crc;
    uint32_t _baud;
    crsfLinkStatistics_t _linkStatistics;
    
    void handleByteReceived();
    void shiftRxBuffer(uint8_t cnt);
    void checkPacketTimeout();
    void checkLinkDown();
};

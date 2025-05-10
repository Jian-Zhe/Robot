#pragma once

#include "../CrsfSerial/CrsfSerial.h"

#define CHANNEL_SEND_INTERVAL_US 300U

class CrsfRx : public CrsfSerial
{
public:

    CrsfRx(serialib &port, uint32_t baud = CRSF_BAUDRATE);

    void loop_rx();

    void queuePacketGps();

    // Return current channel value (1-based) in us
    int getChannel(unsigned int ch) const { return _channels[ch - 1]; }
    
    // CRSF Packet Callbacks
    void (*onPacketChannels)();

protected:
    virtual void processPacketIn(uint8_t len);

private:
    int _channels[CRSF_NUM_CHANNELS];

    // Packet Handlers
    void packetChannelsPacked(const crsf_header_t *p);

    void checkSendTelemetry();

    uint32_t _lastChannelsPacket;
};

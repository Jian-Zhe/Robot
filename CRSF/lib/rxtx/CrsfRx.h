/**
 * CrsfRx.h
 * Copyright (c) 2025 Jian-Zhe, Su
 * This file is part of CRSF.
 * CRSF is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This file includes code from ServoF (GPLv3), see NOTICE for details.
 */

#pragma once

#include "crsf/CrsfSerial/CrsfSerial.h"

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

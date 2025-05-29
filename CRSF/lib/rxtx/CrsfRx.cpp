/**
 * CrsfRx.cpp
 * Copyright (c) 2025 Jian-Zhe, Su
 * This file is part of CRSF.
 * CRSF is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This file includes code from ServoF (GPLv3), see NOTICE for details.
 */
#include "CrsfRx.h"

CrsfRx::CrsfRx(serialib &port, uint32_t baud) :
    CrsfSerial(port, baud)
{}

void CrsfRx::processPacketIn(uint8_t len)
{
    const crsf_header_t *hdr = (crsf_header_t *)_rxBuf;
    switch (hdr->type)
    {
    case CRSF_FRAMETYPE_RC_CHANNELS_PACKED:
        packetChannelsPacked(hdr);
        break;
    case CRSF_FRAMETYPE_LINK_STATISTICS:
        packetLinkStatistics(hdr);
        break;
    }
}

void CrsfRx::loop_rx()
{
    handleSerialIn();
    checkSendTelemetry();
}

void CrsfRx::checkSendTelemetry()
{
    static uint64_t lastSend;
    uint64_t now = millis();
    if (now - lastSend < CHANNEL_SEND_INTERVAL_US)
        return;
    lastSend = now;

    queuePacketGps();
}

void CrsfRx::queuePacketGps()
{
    crsf_sensor_gps_t gps = {};
    gps.latitude = 5269;
    gps.longitude = 1314;
    queuePacket(CRSF_FRAMETYPE_GPS, &gps, sizeof(gps));
}

void CrsfRx::packetChannelsPacked(const crsf_header_t *p)
{
    // Unpack CRSF channel data stored bytewise 11 bits / channel
    // Code assumes there is enough payload for all the channels
    constexpr unsigned inputMask = (1 << CRSF_BITS_PER_CHANNEL) - 1;
    const uint8_t *buf = p->data;
    unsigned scratch = 0; 
    unsigned bitsInScratch = 0;
    for (unsigned ch=0; ch<CRSF_NUM_CHANNELS; ++ch)
    {
        while (bitsInScratch < CRSF_BITS_PER_CHANNEL)
        {
            scratch |= (*buf++) << bitsInScratch;
            bitsInScratch += 8;
        }

        _channels[ch] = CRSF_to_US(scratch & inputMask);
        scratch >>= CRSF_BITS_PER_CHANNEL;
        bitsInScratch -= CRSF_BITS_PER_CHANNEL;
    }

    if (!_linkIsUp && onLinkUp)
        onLinkUp();
    _linkIsUp = true;
    _lastReceive = millis();

    if (onPacketChannels)
        onPacketChannels();
}

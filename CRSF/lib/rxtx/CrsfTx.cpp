/**
 * CrsfTx.cpp
 * Copyright (c) 2025 Jian-Zhe, Su
 * This file is part of CRSF.
 * CRSF is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This file includes code from ServoF (GPLv3), see NOTICE for details.
 */
#include "CrsfTx.h"

CrsfTx::CrsfTx(serialib &port, uint32_t baud) :
    CrsfSerial(port, baud)
{}

void CrsfTx::processPacketIn(uint8_t len) 
{
    const crsf_header_t *hdr = (crsf_header_t *)_rxBuf;
    switch (hdr->type)
    {
    case CRSF_FRAMETYPE_GPS:
        packetGps(hdr);
        break;

    case CRSF_FRAMETYPE_LINK_STATISTICS:
        packetLinkStatistics(hdr);
        break;
    }
    
}

void CrsfTx::loop_tx()
{
    // Check for incoming data
    handleSerialIn();

    // Check for outgoing data
    checkSendChannels();
}

void CrsfTx::checkSendChannels()
{
    static uint64_t lastSend;
    uint64_t now = millis();
    if (now - lastSend < CHANNEL_SEND_INTERVAL_US)
        return;
    lastSend = now;

    // insert mixer logic here and use crsf.setChannel() to set all the channel values
    // This just increments channel 1/8 value by 1/2us every time, wrapping around
    auto mixer = [this](unsigned int ch, unsigned int step) {
        unsigned val = getChannel(ch) + step;
        if (val > 2012)
            val = 988;
        setChannel(ch, val);
    };

    mixer(1, 1);
    mixer(8, 2);
    // End mixer code
    setChannel(5, 2000);
    queuePacketChannels();
}

/***
 * @brief: Write 16 channels out as a handset would
 * @details:    TX ONLY! Packs the us channel data which has been set with setChannel()
 *              and writes it to the connected CRSF transmitter module.
 *              External CRSF transmitters are usually half duplex inverted
 *              and also scheduled by the module sending timing data, this
 *              code handles none of that. This will, however, get a
 *              transmitter to start transmitting channels.
 */
void CrsfTx::queuePacketChannels()
{
   // 11 bits per channel * 16 channels = 176 bits = 22 bytes
    uint8_t packedChannels[(CRSF_NUM_CHANNELS * CRSF_BITS_PER_CHANNEL + 7) / 8];
    uint8_t *pbuf = packedChannels;

    // Pack 11 bit channels low byte first into packedChannels
    uint32_t scratch = 0;
    uint32_t bitsInScratch = 0;
    for (unsigned ch=0; ch<CRSF_NUM_CHANNELS; ++ch)
    {
        uint32_t crsfVal = US_to_CRSF(_channels[ch]);
        scratch |= crsfVal << bitsInScratch;
        bitsInScratch += CRSF_BITS_PER_CHANNEL;
        while (bitsInScratch > 8)
        {
            *pbuf++ = scratch;
            scratch >>= 8;
            bitsInScratch -= 8;
        }
    }

    queuePacket(CRSF_FRAMETYPE_RC_CHANNELS_PACKED, packedChannels, sizeof(packedChannels));
}

void CrsfTx::packetGps(const crsf_header_t *p)
{
    const crsf_sensor_gps_t *gps = (crsf_sensor_gps_t *)p->data;
    // _gpsSensor.latitude = be32toh(gps->latitude);
    // _gpsSensor.longitude = be32toh(gps->longitude);
    // _gpsSensor.groundspeed = be16toh(gps->groundspeed);
    // _gpsSensor.heading = be16toh(gps->heading);
    // _gpsSensor.altitude = be16toh(gps->altitude);
    _gpsSensor.latitude = gps->latitude;
    _gpsSensor.longitude = gps->longitude;
    _gpsSensor.groundspeed = gps->groundspeed;
    _gpsSensor.heading = gps->heading;
    _gpsSensor.altitude = gps->altitude;
    _gpsSensor.satellites = gps->satellites;

    _lastReceive = millis();

    if (onPacketGps)
        onPacketGps(&_gpsSensor);
}
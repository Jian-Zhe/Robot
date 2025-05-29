/**
 * CrsfTx.h
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

#define CHANNEL_SEND_INTERVAL_US 5U

class CrsfTx : public CrsfSerial
{
public:

    CrsfTx(serialib &port, uint32_t baud = CRSF_BAUDRATE);

    void loop_tx();

    const crsf_sensor_gps_t *getGpsSensor() const { return &_gpsSensor; }

    int getChannel(unsigned int ch) const { return _channels[ch - 1]; }
    void setChannel(unsigned int ch, int val) { _channels[ch - 1] = val; }

    void queuePacketChannels();

    // CRSF Packet Callbacks
    void (*onPacketChannels)();
    void (*onPacketGps)(crsf_sensor_gps_t *gpsSensor);

protected:
    virtual void processPacketIn(uint8_t len);

private:
    uint32_t _lastChannelsPacket;
    int _channels[CRSF_NUM_CHANNELS];

    crsf_sensor_gps_t _gpsSensor;

    void checkSendChannels();
    
    void packetGps(const crsf_header_t *p);
};

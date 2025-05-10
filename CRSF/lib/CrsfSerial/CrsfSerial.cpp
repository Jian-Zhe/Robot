#include "CrsfSerial.h"

CrsfSerial::CrsfSerial(serialib &port, uint32_t baud) :
    _port(port), _crc(0xd5), _baud(baud),
    _linkIsUp(false), _lastReceive(0)
{}

void CrsfSerial::begin(uint32_t baud)
{
    if (baud != 0)
        _port.begin(baud);
    else
        _port.begin(_baud);
}

void CrsfSerial::handleSerialIn()
{
    while (_port.available())
    {
        uint8_t b = _port.read();
        _lastReceive = millis();

        _rxBuf[_rxBufPos++] = b;
        handleByteReceived();

        if (_rxBufPos == (sizeof(_rxBuf)/sizeof(_rxBuf[0])))
        {
            // Packet buffer filled and no valid packet found, dump the whole thing
            _rxBufPos = 0;
        }
    }

    checkPacketTimeout();
    checkLinkDown();
}

void CrsfSerial::handleByteReceived()
{
    bool reprocess;
    do
    {
        reprocess = false;
        if (_rxBufPos > 1)
        {
            uint8_t len = _rxBuf[1];
            // Sanity check the declared length isn't outside Type + X{1,CRSF_MAX_PAYLOAD_LEN} + CRC
            // assumes there never will be a CRSF message that just has a type and no data (X)
            if (len < 3 || len > (CRSF_MAX_PAYLOAD_LEN + 2))
            {
                // puts("invalid length");
                shiftRxBuffer(1);
                reprocess = true;
            }

            else if (_rxBufPos >= (len + 2))
            {
                // puts("complete packet");
                uint8_t inCrc = _rxBuf[2 + len - 1];
                uint8_t crc = _crc.calc(&_rxBuf[2], len - 1);
                if (crc == inCrc)
                {
                    processPacketIn(len);
                    shiftRxBuffer(len + 2);
                    reprocess = true;
                }
                else
                {
                    shiftRxBuffer(1);
                    reprocess = true;
                }
            }  // if complete packet
        } // if pos > 1
    } while (reprocess);
}

void CrsfSerial::checkPacketTimeout()
{
    // If we haven't received data in a long time, flush the buffer a byte at a time (to trigger shiftyByte)
    if (_rxBufPos > 0 && millis() - _lastReceive > CRSF_PACKET_TIMEOUT_MS)
        while (_rxBufPos)
            shiftRxBuffer(1);
}

void CrsfSerial::checkLinkDown()
{
    if (_linkIsUp && millis() - _lastReceive > CRSF_FAILSAFE_STAGE1_MS)
    {
        if (onLinkDown)
            onLinkDown();
        _linkIsUp = false;
    }
}

void CrsfSerial::processPacketIn(uint8_t len)
{
    const crsf_header_t *hdr = (crsf_header_t *)_rxBuf;
    switch (hdr->type)
    {
    case CRSF_FRAMETYPE_LINK_STATISTICS:
        packetLinkStatistics(hdr);
        break;
    }
}

// Shift the bytes in the RxBuf down by cnt bytes
void CrsfSerial::shiftRxBuffer(uint8_t cnt)
{
    // If removing the whole thing, just set pos to 0
    if (cnt >= _rxBufPos)
    {
        _rxBufPos = 0;
        return;
    }

    // Otherwise do the slow shift down
    uint8_t *src = &_rxBuf[cnt];
    uint8_t *dst = &_rxBuf[0];
    _rxBufPos -= cnt;
    uint8_t left = _rxBufPos;
    while (left--)
        *dst++ = *src++;
}

void CrsfSerial::packetLinkStatistics(const crsf_header_t *p)
{
    const crsfLinkStatistics_t *link = (crsfLinkStatistics_t *)p->data;
    memcpy(&_linkStatistics, link, sizeof(_linkStatistics));

    // This is for the TX, but checkLinkDown() will keep triggering
    // due to no channels coming in, so this is disabled for now
    // because the timeout needs to be a function of packet rate
    // bool linkIsUp = _linkStatistics.uplink_Link_quality != 0;
    // if (linkIsUp != _linkIsUp)
    // {
    //     _linkIsUp = linkIsUp;
    //     if (_linkIsUp && onLinkUp)
    //         onLinkUp();
    //     else if (!_linkIsUp && onLinkDown)
    //         onLinkDown();
    // }
    _lastReceive = millis();
    
    if (onPacketLinkStatistics)
        onPacketLinkStatistics(&_linkStatistics);
}

void CrsfSerial::write(uint8_t b)
{
    _port.write(b);
}

void CrsfSerial::write(const uint8_t *buf, size_t len)
{
    _port.write(buf, len);
}

void CrsfSerial::queuePacket(uint8_t type, const void *payload, uint8_t len)
{
    if (len > CRSF_MAX_PAYLOAD_LEN)
        return;

    uint8_t buf[CRSF_MAX_PACKET_SIZE];
    buf[0] = CRSF_SYNC_BYTE;
    buf[1] = len + 2; // type + payload + crc
    buf[2] = type;
    memcpy(&buf[3], payload, len);
    buf[len+3] = _crc.calc(&buf[2], len + 1);

    write(buf, len + 4);
}
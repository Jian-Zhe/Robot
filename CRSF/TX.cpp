#include "lib/serial/serialib.h"
#include "lib/rxtx/CrsfTx.h"
#include <unistd.h>
#include <stdio.h>

#define SERIAL_PORT "/dev/ttyACM0"

serialib serial(SERIAL_PORT);
CrsfTx crsf(serial, 115200);

void packetLinkStatistics(crsfLinkStatistics_t *ls)
{
    printf("Link Quality=");
    printf("%d\n", ls->uplink_Link_quality);
}

void onPacketGps(crsf_sensor_gps_t *gpsSensor)
{
    printf("GPS: %d %d\n", gpsSensor->latitude, gpsSensor->longitude);
}

static void setupCrsfChannels()
{
    for (unsigned ch=1; ch<=CRSF_NUM_CHANNELS; ++ch)
        crsf.setChannel(ch, 1500);
}

int main( /*int argc, char *argv[]*/)
{
    // Open the serial port
    crsf.begin(115200);
    // crsf.onPacketLinkStatistics = &packetLinkStatistics;
    crsf.onPacketGps = &onPacketGps;
    
    setupCrsfChannels();

    while(1) {
        crsf.loop_tx();
    }

    return 0 ;
}

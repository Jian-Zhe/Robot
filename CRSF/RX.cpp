#include "serial/serialib.h"
#include "lib/rxtx/CrsfRx.h"
#include <unistd.h>
#include <stdio.h>

#define SERIAL_PORT "/dev/ttyACM1"

serialib serial(SERIAL_PORT);
CrsfRx crsf(serial, 115200);

void packetChannels()
{
    printf("CH1=");
    printf("%d\n", crsf.getChannel(1));
    printf("CH2=");
    printf("%d\n", crsf.getChannel(2));
    printf("CH3=");
    printf("%d\n", crsf.getChannel(3));
    printf("CH4=");
    printf("%d\n", crsf.getChannel(4));
    printf("CH5=");
    printf("%d\n", crsf.getChannel(5));
    printf("CH6=");
    printf("%d\n", crsf.getChannel(6));
    printf("CH7=");
    printf("%d\n", crsf.getChannel(7));
    printf("CH8=");
    printf("%d\n", crsf.getChannel(8));
    printf("CH9=");
    printf("%d\n", crsf.getChannel(9));
    printf("CH10=");
    printf("%d\n", crsf.getChannel(10));
    printf("CH11=");
    printf("%d\n", crsf.getChannel(11));
    printf("CH12=");
    printf("%d\n", crsf.getChannel(12));
    printf("CH13=");
    printf("%d\n", crsf.getChannel(13));
    printf("CH14=");
    printf("%d\n", crsf.getChannel(14));
    printf("CH15=");
    printf("%d\n", crsf.getChannel(15));
    printf("CH16=");
    printf("%d\n", crsf.getChannel(16));
}

void packetLinkStatistics(crsfLinkStatistics_t *ls)
{
    printf("Link Quality=");
    printf("%d\n", ls->uplink_Link_quality);
    printf("Uplink RSSI=");
    printf("%d\n", ls->uplink_RSSI_1);
    printf("Uplink SNR=");
    printf("%d\n", ls->uplink_SNR);
    printf("Uplink TX Power=");
    printf("%d\n", ls->uplink_TX_Power);
    printf("Downlink RSSI=");
    printf("%d\n", ls->downlink_RSSI);
    printf("Downlink SNR=");
    printf("%d\n", ls->downlink_SNR);
    printf("Downlink Link Quality=");
    printf("%d\n", ls->downlink_Link_quality);
    printf("Active Antenna=");
    printf("%d\n", ls->active_antenna);
    printf("RF Mode=");
    printf("%d\n", ls->rf_Mode);
}

int main( /*int argc, char *argv[]*/)
{
    // Open the serial port
    crsf.begin(115200);
    // crsf.onPacketChannels = &packetChannels;
    // crsf.onPacketLinkStatistics = &packetLinkStatistics;

    while(1) {
        crsf.loop_rx();
    }

    return 0 ;
}

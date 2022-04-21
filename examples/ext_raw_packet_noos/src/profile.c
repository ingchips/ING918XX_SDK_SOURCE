#include <stdio.h>
#include <string.h>
#include "platform_api.h"
#include "ingsoc.h"

#define CH_ID                   10

struct ll_raw_packet *raw_packet;
char data[300];

void show_rx(struct ll_raw_packet *packet)
{
    uint64_t air_time;
    uint8_t header;
    int len;
    int rssi;
    
    if (ll_raw_packet_get_rx_data(packet, &air_time, &header, data, &len, &rssi) == 0)
    {
        platform_printf("T: %llu\n", air_time);
        platform_printf("H: 0x%02x\n", header);
        platform_printf("RSSI: %ddBm\n", rssi);
        data[len] = '\0';
        platform_printf("Rx: ");
        platform_printf(data);
        platform_printf("\n");
    }
    else
        platform_printf("Rx error\n");
}

void packet_action(void)
{
#if (FOR_TX)
    static int counter = 0;
    platform_printf("Tx done\n");
    sprintf(data, "RAW PACKET #%d", counter++);

    ll_raw_packet_set_tx_data(raw_packet, counter & 0x7f, data, strlen(data));
    ll_raw_packet_send(raw_packet, platform_get_us_time() + 500000);
#else
    show_rx(raw_packet);

    ll_raw_packet_recv(raw_packet, platform_get_us_time() + 300, 1000 * 1000);
#endif
}

void on_raw_packet_done(struct ll_raw_packet *packet, void *user_data)
{
    packet_action();
}

void init(void)
{
    raw_packet = ll_raw_packet_alloc(FOR_TX, on_raw_packet_done, NULL);
    ll_raw_packet_set_param(raw_packet,
                      3,        // tx_power
                      CH_ID,    // phy_channel_id
                      1,        // phy
                      0x8E89BED6,//0xabcdef,
                      0x555555);
}

void platform_timer_cb(void)
{
    platform_printf("platform_timer\n");
    platform_set_timer(platform_timer_cb, 1600);
}

void noos_start(void)
{
    platform_init_controller();

    init();
    packet_action();
    platform_set_timer(platform_timer_cb, 1600);

    while (1)
    {
        platform_controller_run();
    }
}

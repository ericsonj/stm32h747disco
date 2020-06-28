#include <AudioPlayer.h>

/*
 * App.c
 *
 *  Created on: Jan 16, 2020
 *      Author: Ericson Joseph
 */

/*<test>*/
#include "App.h"
/*</test>*/
#include "FreeRTOSConfig.h"
#include "ethernetif.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/udp.h"
#include "httpserver_netconn.h"
#include "app_ethernet.h"
#include <string.h>

#include "cmsis_os.h"
#include "lwipopts.h"
#include "lcd_trace.h"
#include "lwip/ip_addr.h"
#include "LcdLog.h"
#include <stdint.h>
#include "SDFatFs.h"
#include "AudioPlayer.h"
#include "QSPI_Flash.h"

#ifndef VERSION
#    define VERSION "X.X.X"
#endif

struct netif    gnetif; /* network interface structure */
static uint16_t count = 0;

static void APP_task(const void* arg);
static void APP_InternetTest(const void* arg);
static void Netif_Config(void);

unsigned int yaffs_trace_mask;

void APP_init() {
    osThreadDef(App_Thread, APP_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 5);
    osThreadCreate(osThread(App_Thread), NULL);
}

/**
 * Application task loop
 * @param arg
 */
void APP_task(const void* arg) {

    /* Create tcp_ip stack thread */
    tcpip_init(NULL, NULL);

    /* Initialize the LwIP stack */
    Netif_Config();

    /* Initialize webserver demo */
    http_server_netconn_init();

    SDFatFs_Init();

    QSPIFlash_init();

    osThreadDef(InternetTest_Thread, APP_InternetTest, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 8);
    osThreadCreate(osThread(InternetTest_Thread), NULL);

    for (;;) {
        /* Delete the Init Thread */
        osThreadTerminate(NULL);
    }
}

/**
 * @brief  Initializes the lwIP stack
 * @param  None
 * @retval None
 */
static void Netif_Config(void) {

    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

#if LWIP_DHCP
    ip_addr_set_zero_ip4(&ipaddr);
    ip_addr_set_zero_ip4(&netmask);
    ip_addr_set_zero_ip4(&gw);
#else
    IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif /* LWIP_DHCP */

    /* add the network interface */
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

    /*  Registers the default network interface. */
    netif_set_default(&gnetif);

    ethernet_link_status_updated(&gnetif);

#if LWIP_NETIF_LINK_CALLBACK
    netif_set_link_callback(&gnetif, ethernet_link_status_updated);

    osThreadDef(EthLink, ethernet_link_thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2);
    osThreadCreate(osThread(EthLink), &gnetif);
#endif

#if LWIP_DHCP
    /* Start DHCPClient */
    osThreadDef(DHCP, DHCP_Thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 2);
    osThreadCreate(osThread(DHCP), &gnetif);
#endif
}

void udp_echo_recv(void* arg, struct udp_pcb* pcb, struct pbuf* p,
                   const ip_addr_t* addr, u16_t port) {
    if (p != NULL) {
        char* msg = (char*)pvPortMalloc(p->len);
        memcpy(msg, p->payload, p->len);
        LCDLog_RLog(1, "RECV OK %s %d", CODE_VERSION, count++);
#if IS_BETA
        const char beta[] = " BETA \n";
        LCD_UsrLog(beta);
#endif
        vPortFree(msg);
        pbuf_free(p);
    }
}

void APP_InternetTest(const void* arg) {

    struct udp_pcb* ptel_pcb;
    char            msg[] = "testing";
    struct pbuf*    p;
    ip4_addr_t      dstAddr;
    ip4addr_aton("207.246.65.130", &dstAddr);

    ptel_pcb = udp_new();

    udp_bind(ptel_pcb, IP_ADDR_ANY, 5000);
    udp_recv(ptel_pcb, udp_echo_recv, NULL);

    while (1) {
        //		if (gnetif.flags & NETIF_FLAG_LINK_UP){
        //			vTaskDelay(1000);
        //			continue;
        //		}
        //Allocate packet buffer
        p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
        memcpy(p->payload, msg, sizeof(msg));
        udp_sendto(ptel_pcb, p, &dstAddr, 5000);
        pbuf_free(p);  //De-allocate packet buffer
        osDelay(1000); //some delay!
    }
}

/*<test>*/

int32_t APP_sum(int32_t a, int32_t b) {
    return a + b;
}

int32_t APP_multi(int32_t a, int32_t b) {
    return a * b;
}

/*</test>*/

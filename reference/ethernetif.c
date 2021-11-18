#include "lan8742.h"

/* ------ Fake Definitions ------ */
#define ETH_PHY_IO_Init (0)
#define ETH_PHY_IO_DeInit (0)
#define ETH_PHY_IO_WriteReg (0)
#define ETH_PHY_IO_ReadReg (0)
#define ETH_PHY_IO_GetTick (0)
#define ETH_FULLDUPLEX_MODE (0)
#define ETH_HALFDUPLEX_MODE (0)
#define ETH_SPEED_100M (0)
#define ETH_SPEED_10M (0)
void* netif;
void EthHandle;
/* ------------------------------ */

lan8742_Object_t LAN8742;

lan8742_IOCtx_t  LAN8742_IOCtx = {ETH_PHY_IO_Init,
                               ETH_PHY_IO_DeInit,
                               ETH_PHY_IO_WriteReg,
                               ETH_PHY_IO_ReadReg,
                               ETH_PHY_IO_GetTick};

static void low_level_init(struct netif *netif)
{
  uint32_t idx, duplex, speed = 0;
  int32_t PHYLinkState;
 
  /* Set PHY IO functions */
  LAN8742_RegisterBusIO(&LAN8742, &LAN8742_IOCtx);
  
  /* Initialize the LAN8742 ETH PHY */
  LAN8742_Init(&LAN8742);
  
  PHYLinkState = LAN8742_GetLinkState(&LAN8742);
  
  /* Get link state */  
  if(PHYLinkState <= LAN8742_STATUS_LINK_DOWN) {
    netif_set_link_down(netif);
    netif_set_down(netif);
  } else {
    switch (PHYLinkState) {
    case LAN8742_STATUS_100MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;
    case LAN8742_STATUS_100MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;
    case LAN8742_STATUS_10MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      break;
    case LAN8742_STATUS_10MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      break;
    default:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;      
    }
  }
}

void ethernet_link_thread( void* argument )
{
  int32_t PHYLinkState;
  uint32_t linkchanged = 0, speed = 0, duplex =0;

  for(;;) {    
    PHYLinkState = LAN8742_GetLinkState(&LAN8742);
    
    if(netif_is_link_up(netif) && (PHYLinkState <= LAN8742_STATUS_LINK_DOWN)) {
      HAL_ETH_Stop_IT(&EthHandle);
      netif_set_down(netif);
      netif_set_link_down(netif);
    } else if(!netif_is_link_up(netif) && (PHYLinkState > LAN8742_STATUS_LINK_DOWN)) {
      switch (PHYLinkState) {
      case LAN8742_STATUS_100MBITS_FULLDUPLEX:
        duplex = ETH_FULLDUPLEX_MODE;
        speed = ETH_SPEED_100M;
        linkchanged = 1;
        break;
      case LAN8742_STATUS_100MBITS_HALFDUPLEX:
        duplex = ETH_HALFDUPLEX_MODE;
        speed = ETH_SPEED_100M;
        linkchanged = 1;
        break;
      case LAN8742_STATUS_10MBITS_FULLDUPLEX:
        duplex = ETH_FULLDUPLEX_MODE;
        speed = ETH_SPEED_10M;
        linkchanged = 1;
        break;
      case LAN8742_STATUS_10MBITS_HALFDUPLEX:
        duplex = ETH_HALFDUPLEX_MODE;
        speed = ETH_SPEED_10M;
        linkchanged = 1;
        break;
      default:
        break;
      }
    }
    osDelay(100);
  }
}
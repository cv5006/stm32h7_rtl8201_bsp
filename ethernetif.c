/*
  ___         _         _        
 |_ _|_ _  __| |_  _ __| |___ ___
  | || ' \/ _| | || / _` / -_|_-<
 |___|_||_\__|_|\_,_\__,_\___/__/

*/
/* USER CODE BEGIN Include for User BSP */
#include "rtl8201f.h"
/* USER CODE END Include for User BSP */



/*
  ___     _          _            _      __ _          
 | _ \_ _(_)_ ____ _| |_ ___   __| |___ / _(_)_ _  ___ 
 |  _/ '_| \ V / _` |  _/ -_) / _` / -_)  _| | ' \/ -_)
 |_| |_| |_|\_/\__,_|\__\___| \__,_\___|_| |_|_||_\___|
                                                       
*/

/* USER CODE BEGIN Private function prototypes for User BSP */
int32_t ETH_PHY_IO_Init(void);
int32_t ETH_PHY_IO_DeInit (void);
int32_t ETH_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal);
int32_t ETH_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal);
int32_t ETH_PHY_IO_GetTick(void);

rtl8201f_Object_t RTL8201F;
rtl8201f_IOCtx_t  RTL8201F_IOCtx = {ETH_PHY_IO_Init,
                                  ETH_PHY_IO_DeInit,
                                  ETH_PHY_IO_WriteReg,
                                  ETH_PHY_IO_ReadReg,
                                  ETH_PHY_IO_GetTick};
/* USER CODE END Private function prototypes for User BSP */



/*
  _                 _                _   ___      _ _   
 | |   _____ __ __ | |   _____ _____| | |_ _|_ _ (_) |_ 
 | |__/ _ \ V  V / | |__/ -_) V / -_) |  | || ' \| |  _|
 |____\___/\_/\_/  |____\___|\_/\___|_| |___|_||_|_|\__|
                                                        
*/

/* USER CODE BEGIN low_level_init Variables Initialization for User BSP */
  ETH_MACConfigTypeDef MACConf;
  int32_t PHYLinkState;
  uint32_t duplex, speed = 0;
/* USER CODE END low_level_init Variables Initialization for User BSP */


/* USER CODE BEGIN low_level_init Code 1 for User BSP */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4|GPIO_PIN_5|PHY_RST_Pin, GPIO_PIN_SET);
  osDelay(100);
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4|GPIO_PIN_5|PHY_RST_Pin, GPIO_PIN_RESET);
  osDelay(100);

  RTL8201F_RegisterBusIO(&RTL8201F, &RTL8201F_IOCtx); // Set PHY IO functions
  if(RTL8201F_Init(&RTL8201F) != RTL8201F_STATUS_OK){ // Initialize the RTL8201F ETH PHY
    Error_Handler();
  }
/* USER CODE END low_level_init Code 1 for User BSP */


/* USER CODE BEGIN low_level_init Code 2 for User BSP */
    PHYLinkState = RTL8201F_GetLinkState(&RTL8201F);    
    if(PHYLinkState <= RTL8201F_STATUS_LINK_DOWN) { // Get link state    
      netif_set_link_down(netif);
      netif_set_down(netif);
    } else {
      switch (PHYLinkState) {
      case RTL8201F_STATUS_100MBITS_FULLDUPLEX:
        duplex = ETH_FULLDUPLEX_MODE;
        speed = ETH_SPEED_100M;
        break;
      case RTL8201F_STATUS_100MBITS_HALFDUPLEX:
        duplex = ETH_HALFDUPLEX_MODE;
        speed = ETH_SPEED_100M;
        break;
      case RTL8201F_STATUS_10MBITS_FULLDUPLEX:
        duplex = ETH_FULLDUPLEX_MODE;
        speed = ETH_SPEED_10M;
        break;
      case RTL8201F_STATUS_10MBITS_HALFDUPLEX:
        duplex = ETH_HALFDUPLEX_MODE;
        speed = ETH_SPEED_10M;
        break;
      default:
        duplex = ETH_FULLDUPLEX_MODE;
        speed = ETH_SPEED_100M;
        break;
      }

    /* Get MAC Config MAC */
      HAL_ETH_GetMACConfig(&heth, &MACConf);
      MACConf.DuplexMode = duplex;
      MACConf.Speed = speed;
      HAL_ETH_SetMACConfig(&heth, &MACConf);

      HAL_ETH_Start_IT(&heth);
      netif_set_up(netif);
      netif_set_link_up(netif);
    }
/* USER CODE END low_level_init Code 2 for User BSP */



/*
  ___ _  _ ___   ___ ___    ___             _   _             
 | _ \ || |_ _| |_ _/ _ \  | __|  _ _ _  __| |_(_)___ _ _  ___
 |  _/ __ || |   | | (_) | | _| || | ' \/ _|  _| / _ \ ' \(_-<
 |_| |_||_|___| |___\___/  |_| \_,_|_||_\__|\__|_\___/_||_/__/
                                                              
*/

/* USER CODE BEGIN PHI IO Functions for User BSP */
/**
  * @brief  Initializes the MDIO interface GPIO and clocks.
  * @param  None
  * @retval 0 if OK, -1 if ERROR
  */
int32_t ETH_PHY_IO_Init(void)
{
  /* We assume that MDIO GPIO configuration is already done
     in the ETH_MspInit() else it should be done here
  */

  /* Configure the MDIO Clock */
  HAL_ETH_SetMDIOClockRange(&heth);

  return 0;
}

/**
  * @brief  De-Initializes the MDIO interface .
  * @param  None
  * @retval 0 if OK, -1 if ERROR
  */
int32_t ETH_PHY_IO_DeInit (void)
{
  return 0;
}

/**
  * @brief  Read a PHY register through the MDIO interface.
  * @param  DevAddr: PHY port address
  * @param  RegAddr: PHY register address
  * @param  pRegVal: pointer to hold the register value
  * @retval 0 if OK -1 if Error
  */
int32_t ETH_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal)
{
  if(HAL_ETH_ReadPHYRegister(&heth, DevAddr, RegAddr, pRegVal) != HAL_OK) {
    return -1;
  }else{
    return 0;
  }
}

/**
  * @brief  Write a value to a PHY register through the MDIO interface.
  * @param  DevAddr: PHY port address
  * @param  RegAddr: PHY register address
  * @param  RegVal: Value to be written
  * @retval 0 if OK -1 if Error
  */
int32_t ETH_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal)
{
  if(HAL_ETH_WritePHYRegister(&heth, DevAddr, RegAddr, RegVal) != HAL_OK) {
    return -1;
  }else{
    return 0;
  }    
}

/**
  * @brief  Get the time in millisecons used for internal PHY driver process.
  * @retval Time value
  */
int32_t ETH_PHY_IO_GetTick(void)
{
  return HAL_GetTick();
}
/* USER CODE END PHI IO Functions for User BSP */



/*
  ___ _____ _  _   _    _      _     _____ _                    _ 
 | __|_   _| || | | |  (_)_ _ | |__ |_   _| |_  _ _ ___ __ _ __| |
 | _|  | | | __ | | |__| | ' \| / /   | | | ' \| '_/ -_) _` / _` |
 |___| |_| |_||_| |____|_|_||_|_\_\   |_| |_||_|_| \___\__,_\__,_|
                                                                  
*/

/* USER CODE BEGIN ETH link init */
  ETH_MACConfigTypeDef MACConf;
  int32_t PHYLinkState;
  uint32_t linkchanged = 0, speed = 0, duplex =0;
  struct netif *netif = (struct netif *) argument;
/* USER CODE END ETH link init */

/* USER CODE BEGIN ETH link Thread core code for User BSP */
  PHYLinkState = RTL8201F_GetLinkState(&RTL8201F);

  if(netif_is_link_up(netif) && (PHYLinkState <= RTL8201F_STATUS_LINK_DOWN)) {
    HAL_ETH_Stop_IT(&heth);
    netif_set_down(netif);
    netif_set_link_down(netif);
  } else if(!netif_is_link_up(netif) && (PHYLinkState > RTL8201F_STATUS_LINK_DOWN)) {
    switch (PHYLinkState) {
    case RTL8201F_STATUS_100MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      linkchanged = 1;
      break;
    case RTL8201F_STATUS_100MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      linkchanged = 1;
      break;
    case RTL8201F_STATUS_10MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      linkchanged = 1;
      break;
    case RTL8201F_STATUS_10MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      linkchanged = 1;
      break;
    default:
      break;
    }

    if(linkchanged) {
      /* Get MAC Config MAC */
      HAL_ETH_GetMACConfig(&heth, &MACConf);
      MACConf.DuplexMode = duplex;
      MACConf.Speed = speed;
      HAL_ETH_SetMACConfig(&heth, &MACConf);

      HAL_ETH_Start_IT(&heth);
      netif_set_up(netif);
      netif_set_link_up(netif);
    }
  }
/* USER CODE END ETH link Thread core code for User BSP */

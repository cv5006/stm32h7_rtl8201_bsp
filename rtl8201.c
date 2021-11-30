#include "rtl8201.h"
#include "rtl8201_regmap.h"

#define RTL8201_SW_RESET_TIMEOUT ((uint32_t)500U)
#define RTL8201_INIT_TIMEOUT     ((uint32_t)2000U)
#define RTL8201_PHY_ADDR         ((uint32_t)0b00U)

typedef enum{
  Set,
  Reset,
} RegSettingMode;

static int32_t RegSetting(rtl8201_Object_t *pObj, uint16_t addr, uint16_t mask, uint16_t flag, RegSettingMode rsm)
{
  uint32_t regval;

  if(pObj->IO.ReadReg(pObj->DevAddr, addr, &regval) < 0){
    return RTL8201_STATUS_READ_ERROR;
  }

  regval &= ~mask;
  if(rsm == Set){
    regval |= flag;
  } else {
    regval &= ~flag;
  }
    
  if(pObj->IO.WriteReg(pObj->DevAddr, addr, regval) < 0){
      return RTL8201_STATUS_WRITE_ERROR;
  }

  if(pObj->IO.ReadReg(pObj->DevAddr, addr, &regval) < 0){
    return RTL8201_STATUS_READ_ERROR;
  }

  return RTL8201_STATUS_OK;
}

int32_t RTL8201_RegisterBusIO(rtl8201_Object_t *pObj, rtl8201_IOCtx_t *ioctx)
{
  if(!pObj || !ioctx->ReadReg || !ioctx->WriteReg || !ioctx->GetTick) {
    return RTL8201_STATUS_ERROR;
  }
  
  pObj->IO.Init = ioctx->Init;
  pObj->IO.DeInit = ioctx->DeInit;
  pObj->IO.ReadReg = ioctx->ReadReg;
  pObj->IO.WriteReg = ioctx->WriteReg;
  pObj->IO.GetTick = ioctx->GetTick;
  
  return RTL8201_STATUS_OK;
}

int32_t RTL8201_Init(rtl8201_Object_t *pObj)
 {
   uint32_t readval = 0;
   uint32_t tickstart = 0, regvalue = 0, addr = 0;
   int32_t status = RTL8201_STATUS_OK;
   
   if(pObj->Is_Initialized == 0) {
     if(pObj->IO.Init != 0) {       
       pObj->IO.Init(); // GPIO and Clocks initialization
     }
   }
   
   pObj->DevAddr = RTL8201_PHY_ADDR;

   if(status == RTL8201_STATUS_OK) { // if device address is matched
     if(pObj->IO.WriteReg(pObj->DevAddr, RTL8201_BCR, RTL8201_BCR_SOFT_RST) >= 0) { // set a software reset
       if(pObj->IO.ReadReg(pObj->DevAddr, RTL8201_BCR, &regvalue) >= 0) { // get software reset status
         tickstart = pObj->IO.GetTick();
         while(regvalue & RTL8201_BCR_SOFT_RST){ // wait until software reset is done or timeout occured
           if((pObj->IO.GetTick() - tickstart) <= RTL8201_SW_RESET_TIMEOUT) {
              if(pObj->IO.ReadReg(pObj->DevAddr, RTL8201_BCR, &regvalue) < 0) {
                 status = RTL8201_STATUS_READ_ERROR;
                 break;
             }
           } else {
             status = RTL8201_STATUS_RESET_TIMEOUT;
             break;
           }
         }
       } else {
         status = RTL8201_STATUS_READ_ERROR;
       }
     } else {
       status = RTL8201_STATUS_WRITE_ERROR;
     }
   }

  RegSetting(pObj, RTL8201_PSM, RTL8201_PSM_MASK, RTL8201_PSM_SET_PSM, Reset); // disable power saving mode

  RegSetting(pObj, RTL8201_PSR, RTL8201_PSR_MASK, RTL8201_PSR_P7, Set); // set page 7
  RegSetting(pObj, RTL8201_P7_RMSR, RTL8201_P7_RMSR_RMII_MODE_MASK, RTL8201_P7_RMSR_SET_RMII_MODE, Set); // set RMII mode
  RegSetting(pObj, RTL8201_P7_LED, RTL8201_P7_LED_MASK, RTL8201_P7_LED_ACK_ALL_LINK_100, Set); // set LED mode
  
  RegSetting(pObj, RTL8201_PSR, RTL8201_PSR_MASK, RTL8201_PSR_P0, Set); // set page 0



   if(status == RTL8201_STATUS_OK) {
     tickstart =  pObj->IO.GetTick();
     
     /* Wait for 2s to perform initialization */
     while((pObj->IO.GetTick() - tickstart) <= RTL8201_INIT_TIMEOUT) {}
     pObj->Is_Initialized = 1;
   }
   
   return status;
 }

 int32_t RTL8201_GetLinkState(rtl8201_Object_t *pObj)
{
  uint32_t readval = 0;
  
  /* Read Status register  */
  if(pObj->IO.ReadReg(pObj->DevAddr, RTL8201_BSR, &readval) < 0) {
    return RTL8201_STATUS_READ_ERROR;
  }
  
  /* Read Status register again */
  if(pObj->IO.ReadReg(pObj->DevAddr, RTL8201_BSR, &readval) < 0) {
    return RTL8201_STATUS_READ_ERROR;
  }
  
  if((readval & RTL8201_BSR_LINK_STATUS) == 0) {
    /* Return Link Down status */
    return RTL8201_STATUS_LINK_DOWN;    
  }
  
  /* Check Auto negotiaition */
  if(pObj->IO.ReadReg(pObj->DevAddr, RTL8201_BCR, &readval) < 0) {
    return RTL8201_STATUS_READ_ERROR;
  }
  
  if((readval & RTL8201_BCR_AUTONEGO_EN) != RTL8201_BCR_AUTONEGO_EN) { /* Auto Nego NOT enabled */    
    if(((readval & RTL8201_BCR_SPEED_SELECT) == RTL8201_BCR_SPEED_SELECT) && ((readval & RTL8201_BCR_DUPLEX_MODE) == RTL8201_BCR_DUPLEX_MODE)) {
      return RTL8201_STATUS_100MBITS_FULLDUPLEX;
    } else if ((readval & RTL8201_BCR_SPEED_SELECT) == RTL8201_BCR_SPEED_SELECT) {
      return RTL8201_STATUS_100MBITS_HALFDUPLEX;
    } else if ((readval & RTL8201_BCR_DUPLEX_MODE) == RTL8201_BCR_DUPLEX_MODE) {
      return RTL8201_STATUS_10MBITS_FULLDUPLEX;
    } else {
      return RTL8201_STATUS_10MBITS_HALFDUPLEX;
    } 
  } 		
  // } else { /* Auto Nego enabled */    
  //   if(pObj->IO.ReadReg(pObj->DevAddr, RTL8201_PHYSCSR, &readval) < 0) {
  //     return RTL8201_STATUS_READ_ERROR;
  //   }    
  //   /* Check if auto nego not done */
  //   if((readval & RTL8201_PHYSCSR_AUTONEGO_DONE) == 0) {
  //     return RTL8201_STATUS_AUTONEGO_NOTDONE;
  //   }    
  //   if((readval & RTL8201_PHYSCSR_HCDSPEEDMASK) == RTL8201_PHYSCSR_100BTX_FD) {
  //     return RTL8201_STATUS_100MBITS_FULLDUPLEX;
  //   } else if ((readval & RTL8201_PHYSCSR_HCDSPEEDMASK) == RTL8201_PHYSCSR_100BTX_HD) {
  //     return RTL8201_STATUS_100MBITS_HALFDUPLEX;
  //   } else if ((readval & RTL8201_PHYSCSR_HCDSPEEDMASK) == RTL8201_PHYSCSR_10BT_FD) {
  //     return RTL8201_STATUS_10MBITS_FULLDUPLEX;
  //   } else {
  //     return RTL8201_STATUS_10MBITS_HALFDUPLEX;
  //   }				
  // }
}
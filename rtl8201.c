#include "rtl8201.h"


/*
  ___     _          _         ___             _   _             
 | _ \_ _(_)_ ____ _| |_ ___  | __|  _ _ _  __| |_(_)___ _ _  ___
 |  _/ '_| \ V / _` |  _/ -_) | _| || | ' \/ _|  _| / _ \ ' \(_-<
 |_| |_| |_|\_/\__,_|\__\___| |_| \_,_|_||_\__|\__|_\___/_||_/__/
                                                                                                                             
*/

static void Delay(rtl8201_Object_t *pObj, uint32_t time)
{
    uint32_t tickstart =  pObj->IO.GetTick();
    while((pObj->IO.GetTick() - tickstart) <= time) {}
}


static int32_t ReadReg(rtl8201_Object_t *pObj, uint16_t addr, uint32_t* readval)
{
    if(pObj->IO.ReadReg(pObj->DevAddr, addr, readval) < 0) {
        return RTL8201_STATUS_READ_ERROR;
    } else {
        return RTL8201_STATUS_OK;
    }
}

static int32_t WriteReg(rtl8201_Object_t *pObj, uint16_t addr, uint32_t writeval)
{
    if(pObj->IO.WriteReg(pObj->DevAddr, addr, writeval) < 0) {
        return RTL8201_STATUS_WRITE_ERROR;
    } else {
        return RTL8201_STATUS_OK;
    }
}

static int32_t RegSetting(rtl8201_Object_t *pObj, uint16_t addr, uint16_t mask, uint16_t flag)
{
    uint32_t regval;
    int32_t status = RTL8201_STATUS_OK;

    status = ReadReg(pObj, addr, &regval);

    regval &= ~mask;
    regval |= flag;

    status = WriteReg(pObj, addr, regval);

#ifdef DEBUG
    uint32_t readval;
    status = ReadReg(pObj, addr, &readval);
#endif

    return status;
}

static int32_t SoftReset(rtl8201_Object_t *pObj, uint32_t timeout)
{
    uint32_t tickstart = 0, regval = 0;
    int32_t status = RTL8201_STATUS_OK;

    status = WriteReg(pObj, RTL8201_BCR, RTL8201_BCR_SOFT_RST);
    status = ReadReg(pObj, RTL8201_BCR, &regval);

    if(status == RTL8201_STATUS_OK){
        tickstart = pObj->IO.GetTick();
        while(regval & RTL8201_BCR_SOFT_RST){ // wait until software reset is done or timeout occurred
            if((pObj->IO.GetTick() - tickstart) > timeout) {
                status = RTL8201_STATUS_RESET_TIMEOUT;
            } // check timeout
            
            status = ReadReg(pObj, RTL8201_BCR, &regval);
            
            if(status != RTL8201_STATUS_OK){
                break;
            } // check status
        }
    }

    return status;
}


/*
  ___      _    _ _      ___             _   _             
 | _ \_  _| |__| (_)__  | __|  _ _ _  __| |_(_)___ _ _  ___
 |  _/ || | '_ \ | / _| | _| || | ' \/ _|  _| / _ \ ' \(_-<
 |_|  \_,_|_.__/_|_\__| |_| \_,_|_||_\__|\__|_\___/_||_/__/
                                                           
*/

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
    uint32_t regvalue = 0, id = 0;
    int32_t status = RTL8201_STATUS_OK;

    /* GPIO and Clocks initialization */
    if(pObj->Is_Initialized == 0) {
        if(pObj->IO.Init != 0) {       
            pObj->IO.Init(); 
        }
    }

    pObj->DevAddr = RTL8201_PHY_ADDR;
    
    /* Check PHY address*/
    status = ReadReg(pObj, RTL8201_PHYI1R, &regvalue);
    id |= regvalue;
    if(id != RTL8201_PHYI1R_OUI_HIGH) {
        status = RTL8201_STATUS_ADDRESS_ERROR;
    }

    /* Soft reset */
    if(status == RTL8201_STATUS_OK) {
        status = SoftReset(pObj, RTL8201_SW_RESET_TIMEOUT);
    }

    /* Register setting */
    if(status == RTL8201_STATUS_OK) {
        status = RegSetting(pObj, RTL8201_PSR, RTL8201_PSR_MASK, RTL8201_PSR_P7); // set page 7
        Delay(pObj, RTL8201_REG_SETTING_DELAY);
    }

    if(status == RTL8201_STATUS_OK) {
        status = RegSetting(pObj, RTL8201_P7_LED, RTL8201_P7_LED_MASK, RTL8201_P7_LED_ACK_ALL_LINK_100); // set LED mode
        Delay(pObj, RTL8201_REG_SETTING_DELAY);
    }

    if(status == RTL8201_STATUS_OK) {
        status = RegSetting(pObj, RTL8201_P7_RMSR,
            RTL8201_P7_RMSR_RMII_MODE_MASK | RTL8201_P7_RMSR_TX_TIMING_MASK | RTL8201_P7_RMSR_RX_TIMING_MASK,
            RTL8201_P7_RMSR_SET_RMII_MODE  | RTL8201_P7_RMSR_TX_TIMING      | RTL8201_P7_RMSR_RX_TIMING); // set RMII mode
        Delay(pObj, RTL8201_REG_SETTING_DELAY);
    }

    if(status == RTL8201_STATUS_OK) {
        status = RegSetting(pObj, RTL8201_PSR, RTL8201_PSR_MASK, RTL8201_PSR_P0); // set page 0
    }

    /* Delay for initialization */
    if(status == RTL8201_STATUS_OK) {
        Delay(pObj, RTL8201_INIT_TIMEOUT);
        pObj->Is_Initialized = 1;
    }

    return status;
}


int32_t RTL8201_GetLinkState(rtl8201_Object_t *pObj)
{
    uint32_t bsrval = 0, bcrval = 0;
    uint8_t speed100 = 0, duplexfull = 0;
    int32_t status = RTL8201_STATUS_OK;

    /* Read status twice to check current link status */
    status = ReadReg(pObj, RTL8201_BSR, &bsrval);
    status = ReadReg(pObj, RTL8201_BSR, &bsrval);
    
    /* Check link status */
    if(status == RTL8201_STATUS_OK){
        if((bsrval & RTL8201_BSR_LINK_STATUS) == 0) {
            status = RTL8201_STATUS_LINK_DOWN;
        }
    }

    /* Check auto-nego */
    if(status == RTL8201_STATUS_OK){
        status = ReadReg(pObj, RTL8201_BCR, &bcrval);
    }


    if(status == RTL8201_STATUS_OK){
        if((bcrval & RTL8201_BCR_AUTONEGO_EN) != RTL8201_BCR_AUTONEGO_EN) { // Auto-nego NOT enabled
            speed100 = (bcrval & RTL8201_BCR_SPEED_SELECT) == RTL8201_BCR_SPEED_SELECT;
            duplexfull = (bcrval & RTL8201_BCR_DUPLEX_MODE)  == RTL8201_BCR_DUPLEX_MODE;

            if(speed100 && duplexfull){
                status = RTL8201_STATUS_100MBITS_FULLDUPLEX;
            } else if (speed100) {
                status = RTL8201_STATUS_100MBITS_HALFDUPLEX;
            } else if (duplexfull) {
                status = RTL8201_STATUS_10MBITS_FULLDUPLEX;
            } else {
                status = RTL8201_STATUS_10MBITS_HALFDUPLEX;
            }
        } else { // Auto-nego enabled
            if((bsrval & RTL8201_BSR_AUTONEGO_CPLT) == 0){ // check auto-nego done
                status = RTL8201_STATUS_AUTONEGO_NOTDONE;
            }else{
                status = RTL8201_STATUS_100MBITS_FULLDUPLEX;
            }
        }
    }

    return status;
}
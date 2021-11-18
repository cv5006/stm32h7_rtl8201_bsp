#include "rtl8201.h"
#include "rtl8201_regmap.h"

#define RTL8201_SW_RESET_TIMEOUT ((uint32_t)500U)
#define RTL8201_INIT_TIMEOUT     ((uint32_t)2000U)
#define RTL8201_MAX_DEV_ADDR     ((uint32_t)31U)

int32_t  RTL8201_RegisterBusIO(rtl8201_Object_t *pObj, rtl8201_IOCtx_t *ioctx)
{
  if(!pObj || !ioctx->ReadReg || !ioctx->WriteReg || !ioctx->GetTick)
  {
    return RTL8201_STATUS_ERROR;
  }
  
  pObj->IO.Init = ioctx->Init;
  pObj->IO.DeInit = ioctx->DeInit;
  pObj->IO.ReadReg = ioctx->ReadReg;
  pObj->IO.WriteReg = ioctx->WriteReg;
  pObj->IO.GetTick = ioctx->GetTick;
  
  return RTL8201_STATUS_OK;
}
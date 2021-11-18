#define RTL8201_H
#ifdef RTL8201_H

#include <stdint.h>

/*
  ___ _____ _    ___ ___ __  _   ___ _____ _ _____ _   _ ___ 
 | _ \_   _| |  ( _ )_  )  \/ | / __|_   _/_\_   _| | | / __|
 |   / | | | |__/ _ \/ / () | | \__ \ | |/ _ \| | | |_| \__ \
 |_|_\ |_| |____\___/___\__/|_| |___/ |_/_/ \_\_|  \___/|___/
                                                             
*/
#define  RTL8201_STATUS_READ_ERROR            ((int32_t)-5)
#define  RTL8201_STATUS_WRITE_ERROR           ((int32_t)-4)
#define  RTL8201_STATUS_ADDRESS_ERROR         ((int32_t)-3)
#define  RTL8201_STATUS_RESET_TIMEOUT         ((int32_t)-2)
#define  RTL8201_STATUS_ERROR                 ((int32_t)-1)
#define  RTL8201_STATUS_OK                    ((int32_t) 0)
#define  RTL8201_STATUS_LINK_DOWN             ((int32_t) 1)
#define  RTL8201_STATUS_100MBITS_FULLDUPLEX   ((int32_t) 2)
#define  RTL8201_STATUS_100MBITS_HALFDUPLEX   ((int32_t) 3)
#define  RTL8201_STATUS_10MBITS_FULLDUPLEX    ((int32_t) 4)
#define  RTL8201_STATUS_10MBITS_HALFDUPLEX    ((int32_t) 5)
#define  RTL8201_STATUS_AUTONEGO_NOTDONE      ((int32_t) 6)


/*
   ___  _     _        _     __       ___ ___    ___             ___ _               _   
  / _ \| |__ (_)___ __| |_  / _|___  |_ _/ _ \  | __| _  __ ___ / __| |_ _ _ _  _ __| |_ 
 | (_) | '_ \| / -_) _|  _| > _|_ _|  | | (_) | | _| ' \/ _(_-< \__ \  _| '_| || / _|  _|
  \___/|_.__// \___\__|\__| \_____|  |___\___/  |_||_||_\__/__/ |___/\__|_|  \_,_\__|\__|
           |__/                                                                          
*/

typedef int32_t (*rtl8201_Init_Func)     (void); 
typedef int32_t (*rtl8201_DeInit_Func)   (void);
typedef int32_t (*rtl8201_ReadReg_Func)  (uint32_t, uint32_t, uint32_t *);
typedef int32_t (*rtl8201_WriteReg_Func) (uint32_t, uint32_t, uint32_t);
typedef int32_t (*rtl8201_GetTick_Func)  (void);

typedef struct 
{                   
  rtl8201_Init_Func      Init; 
  rtl8201_DeInit_Func    DeInit;
  rtl8201_WriteReg_Func  WriteReg;
  rtl8201_ReadReg_Func   ReadReg; 
  rtl8201_GetTick_Func   GetTick;   
} rtl8201_IOCtx_t;  

  
typedef struct 
{
  uint32_t            DevAddr;
  uint32_t            Is_Initialized;
  rtl8201_IOCtx_t     IO;
  void               *pData;
}rtl8201_Object_t;

/*
  ___             _   _             
 | __|  _ _ _  __| |_(_)___ _ _  ___
 | _| || | ' \/ _|  _| / _ \ ' \(_-<
 |_| \_,_|_||_\__|\__|_\___/_||_/__/
                                    
*/

int32_t RTL8201_RegisterBusIO(rtl8201_Object_t *pObj, rtl8201_IOCtx_t *ioctx);
int32_t RTL8201_Init(rtl8201_Object_t *pObj);
int32_t RTL8201_DeInit(rtl8201_Object_t *pObj);
int32_t RTL8201_DisablePowerDownMode(rtl8201_Object_t *pObj);
int32_t RTL8201_EnablePowerDownMode(rtl8201_Object_t *pObj);
int32_t RTL8201_StartAutoNego(rtl8201_Object_t *pObj);
int32_t RTL8201_GetLinkState(rtl8201_Object_t *pObj);
int32_t RTL8201_SetLinkState(rtl8201_Object_t *pObj, uint32_t LinkState);
int32_t RTL8201_EnableLoopbackMode(rtl8201_Object_t *pObj);
int32_t RTL8201_DisableLoopbackMode(rtl8201_Object_t *pObj);
int32_t RTL8201_EnableIT(rtl8201_Object_t *pObj, uint32_t Interrupt);
int32_t RTL8201_DisableIT(rtl8201_Object_t *pObj, uint32_t Interrupt);
int32_t RTL8201_ClearIT(rtl8201_Object_t *pObj, uint32_t Interrupt);
int32_t RTL8201_GetITStatus(rtl8201_Object_t *pObj, uint32_t Interrupt);

#endif //RTL8201_H

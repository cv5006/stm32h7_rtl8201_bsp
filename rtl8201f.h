#define RTL8201F_H
#ifdef RTL8201F_H

#include <stdint.h>
#include "rtl8201f_regmap.h"

/*
  ___ _        _           
 / __| |_ __ _| |_ _  _ ___
 \__ \  _/ _` |  _| || (_-<
 |___/\__\__,_|\__|\_,_/__/
                                                                                    
*/
#define  RTL8201F_STATUS_READ_ERROR            ((int32_t)-5)
#define  RTL8201F_STATUS_WRITE_ERROR           ((int32_t)-4)
#define  RTL8201F_STATUS_ADDRESS_ERROR         ((int32_t)-3)
#define  RTL8201F_STATUS_RESET_TIMEOUT         ((int32_t)-2)
#define  RTL8201F_STATUS_ERROR                 ((int32_t)-1)
#define  RTL8201F_STATUS_OK                    ((int32_t) 0)
#define  RTL8201F_STATUS_LINK_DOWN             ((int32_t) 1)
#define  RTL8201F_STATUS_100MBITS_FULLDUPLEX   ((int32_t) 2)
#define  RTL8201F_STATUS_100MBITS_HALFDUPLEX   ((int32_t) 3)
#define  RTL8201F_STATUS_10MBITS_FULLDUPLEX    ((int32_t) 4)
#define  RTL8201F_STATUS_10MBITS_HALFDUPLEX    ((int32_t) 5)
#define  RTL8201F_STATUS_AUTONEGO_NOTDONE      ((int32_t) 6)


/*
  ___                         _              
 | _ \__ _ _ _ __ _ _ __  ___| |_ ___ _ _ ___
 |  _/ _` | '_/ _` | '  \/ -_)  _/ -_) '_(_-<
 |_| \__,_|_| \__,_|_|_|_\___|\__\___|_| /__/
                                              */
#define RTL8201F_SW_RESET_TIMEOUT  ((uint32_t)500U)
#define RTL8201F_REG_SETTING_DELAY ((uint32_t)200U)
#define RTL8201F_INIT_TIMEOUT      ((uint32_t)2000U)
#define RTL8201F_PHY_ADDR          ((uint32_t)0b00U)

/*
   ___  _     _        _     __       ___ ___    ___             ___ _               _   
  / _ \| |__ (_)___ __| |_  / _|___  |_ _/ _ \  | __| _  __ ___ / __| |_ _ _ _  _ __| |_ 
 | (_) | '_ \| / -_) _|  _| > _|_ _|  | | (_) | | _| ' \/ _(_-< \__ \  _| '_| || / _|  _|
  \___/|_.__// \___\__|\__| \_____|  |___\___/  |_||_||_\__/__/ |___/\__|_|  \_,_\__|\__|
           |__/                                                                          
*/

typedef int32_t (*rtl8201f_Init_Func)     (void); 
typedef int32_t (*rtl8201f_DeInit_Func)   (void);
typedef int32_t (*rtl8201f_ReadReg_Func)  (uint32_t, uint32_t, uint32_t *);
typedef int32_t (*rtl8201f_WriteReg_Func) (uint32_t, uint32_t, uint32_t);
typedef int32_t (*rtl8201f_GetTick_Func)  (void);

typedef struct 
{                   
  rtl8201f_Init_Func      Init; 
  rtl8201f_DeInit_Func    DeInit;
  rtl8201f_WriteReg_Func  WriteReg;
  rtl8201f_ReadReg_Func   ReadReg; 
  rtl8201f_GetTick_Func   GetTick;   
} rtl8201f_IOCtx_t;  

  
typedef struct 
{
  uint32_t            DevAddr;
  uint32_t            Is_Initialized;
  rtl8201f_IOCtx_t     IO;
  void               *pData;
}rtl8201f_Object_t;

/*
  ___             _   _             
 | __|  _ _ _  __| |_(_)___ _ _  ___
 | _| || | ' \/ _|  _| / _ \ ' \(_-<
 |_| \_,_|_||_\__|\__|_\___/_||_/__/
                                    
*/

int32_t RTL8201F_RegisterBusIO(rtl8201f_Object_t *pObj, rtl8201f_IOCtx_t *ioctx);
int32_t RTL8201F_Init(rtl8201f_Object_t *pObj);
int32_t RTL8201F_DeInit(rtl8201f_Object_t *pObj);
int32_t RTL8201F_DisablePowerDownMode(rtl8201f_Object_t *pObj);
int32_t RTL8201F_EnablePowerDownMode(rtl8201f_Object_t *pObj);
int32_t RTL8201F_StartAutoNego(rtl8201f_Object_t *pObj);
int32_t RTL8201F_GetLinkState(rtl8201f_Object_t *pObj);
int32_t RTL8201F_SetLinkState(rtl8201f_Object_t *pObj, uint32_t LinkState);
int32_t RTL8201F_EnableLoopbackMode(rtl8201f_Object_t *pObj);
int32_t RTL8201F_DisableLoopbackMode(rtl8201f_Object_t *pObj);
int32_t RTL8201F_EnableIT(rtl8201f_Object_t *pObj, uint32_t Interrupt);
int32_t RTL8201F_DisableIT(rtl8201f_Object_t *pObj, uint32_t Interrupt);
int32_t RTL8201F_ClearIT(rtl8201f_Object_t *pObj, uint32_t Interrupt);
int32_t RTL8201F_GetITStatus(rtl8201f_Object_t *pObj, uint32_t Interrupt);

#endif //RTL8201F_H



#include "cm32gpm3_pwrc.h"

//--------------
// TPO
//
void PWRC_Set_TPO(uint8_t tpo)
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->TPO = tpo;
}

uint8_t PWRC_Get_TPO()
{
  while(RtcDomainUpdating == 1);
	return APB_PWRC->TPO;
}

//--------------
// TPD
//
void PWRC_Set_TPD(uint8_t tpd)
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->TPD = tpd;
}

uint8_t PWRC_Get_TPD()
{
  while(RtcDomainUpdating == 1);
	return APB_PWRC->TPD;
}

//--------------
// TAO
//
void PWRC_Set_TAO(uint8_t tao)
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->TAO = tao;
}

uint8_t PWRC_Get_TAO()
{
  while(RtcDomainUpdating == 1);
	return APB_PWRC->TAO;
}

//--------------
// TAD
//
void PWRC_Set_TAD(uint8_t tad)
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->TAD = tad;
}

uint8_t PWRC_Get_TAD()
{
  while(RtcDomainUpdating == 1);
	return APB_PWRC->TAD;
}

//--------------
// TTO
//
void PWRC_Set_TTO(uint8_t tto)
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->TTO = tto;
}

uint8_t PWRC_Get_TTO()
{
  while(RtcDomainUpdating == 1);
	return APB_PWRC->TTO;
}

//--------------
// TTD
//
void PWRC_Set_TTD(uint8_t ttd)
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->TTD = ttd;
}

uint8_t PWRC_Get_TTD()
{
  while(RtcDomainUpdating == 1);
	return APB_PWRC->TTD;
}

//--------------
// DP_SLP (W1)
//
void PWRC_Set_DP_SLP()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->DP_SLP = 1;
}

uint8_t PWRC_Get_DP_SLP()
{
	while(RtcDomainUpdating == 1);
	return APB_PWRC->DP_SLP;
}

//--------------
// WRM_BOOT
//
uint8_t PWRC_Get_WRM_BOOT()
{
	while(RtcDomainUpdating == 1);
	return APB_PWRC->WRM_BOOT;
}

//--------------
// AE
//
void PWRC_Open_AE()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->AE = 1;
}

void PWRC_Close_AE()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->AE = 0;
}

uint8_t PWRC_Get_AE()
{
	while(RtcDomainUpdating == 1);
	return APB_PWRC->AE;
}

//--------------
// TE
//
void PWRC_Open_TE()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->TE = 1;
}

void PWRC_Close_TE()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->TE = 0;
}

uint8_t PWRC_Get_TE()
{
	while(RtcDomainUpdating == 1);
	return APB_PWRC->TE;
}

//--------------
// AE_STAT (RO)
//
uint8_t PWRC_Get_AE_STAT()
{
	while(RtcDomainUpdating == 1);
	return APB_PWRC->AE_STAT;
}

//--------------
// TE_STAT (RO)
//
uint8_t PWRC_Get_TE_STAT()
{
	while(RtcDomainUpdating == 1);
	return APB_PWRC->TE_STAT;
}

//--------------
// UART_EN
//
void PWRC_Open_UART_EN()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->UART_EN = 1;
}

void PWRC_Close_UART_EN()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->UART_EN = 0;
}

uint8_t PWRC_Get_UART_EN()
{
	while(RtcDomainUpdating == 1);
	return APB_PWRC->UART_EN;
}

//--------------
// RTC_M0_EN
//
void PWRC_Open_RTC_M0_EN()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->RTC_M0_EN = 1;
}

void PWRC_Close_RTC_M0_EN()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->RTC_M0_EN = 0;
}

uint8_t PWRC_Get_RTC_M0_EN()
{
	while(RtcDomainUpdating == 1);
	return APB_PWRC->RTC_M0_EN;
}

//--------------
// RTC_M1_EN
//
void PWRC_Open_RTC_M1_EN()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->RTC_M1_EN = 1;
}

void PWRC_Close_RTC_M1_EN()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->RTC_M1_EN = 0;
}

uint8_t PWRC_Get_RTC_M1_EN()
{
	while(RtcDomainUpdating == 1);
	return APB_PWRC->RTC_M1_EN;
}


//--------------
// RTC_EXINT_CTRL
//
void PWRC_Set_EXINT_CTRL(uint8_t ctrl)
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->EXINT_CTRL = ctrl;
}

uint8_t PWRC_Get_EXINT_CTRL()
{
  while(RtcDomainUpdating == 1);
	return APB_PWRC->EXINT_CTRL;
}

// Set EXINT As RtcDomain Interrupt
void PWRC_Set_EXINT_AS_INT()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->EXINT_CTRL &= ~(3 << bsPWRC_EXINT_CTRL_PIN_CTRL); // clear
	while(RtcDomainUpdating == 1);
	APB_PWRC->EXINT_CTRL |= EXINT_CTRL_PIN_AS_INT << bsPWRC_EXINT_CTRL_PIN_CTRL; // write
}

// Set EXINT As RtcDomain GIO
void PWRC_Set_EXINT_AS_GIO()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->EXINT_CTRL &= ~(3 << bsPWRC_EXINT_CTRL_PIN_CTRL); // clear
	while(RtcDomainUpdating == 1);
	APB_PWRC->EXINT_CTRL |= EXINT_CTRL_PIN_AS_GIO << bsPWRC_EXINT_CTRL_PIN_CTRL;  // write
}

// Set EXINT As RtcDomain Debug Port
void PWRC_Set_EXINT_AS_DBG()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->EXINT_CTRL &= ~(3 << bsPWRC_EXINT_CTRL_PIN_CTRL); // clear
	while(RtcDomainUpdating == 1);
	APB_PWRC->EXINT_CTRL |= EXINT_CTRL_PIN_AS_DBG << bsPWRC_EXINT_CTRL_PIN_CTRL;  // write
}

// Set EXINT GIO Output
void PWRC_Set_EXINT_GIO_OUTPUT()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->EXINT_CTRL |= 1 << bsPWRC_EXINT_CTRL_GIO_OE;
}

// Set EXINT GIO Input
void PWRC_Set_EXINT_GIO_INPUT()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->EXINT_CTRL &= ~(1 << bsPWRC_EXINT_CTRL_GIO_OE);
}

// Set EXINT GIO Output High
void PWRC_Set_EXINT_GIO_OUTPUT_HIGH()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->EXINT_CTRL |= 1 << bsPWRC_EXINT_CTRL_GIO_O;
}

// Set EXINT GIO Output Low
void PWRC_Set_EXINT_GIO_OUTPUT_LOW()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->EXINT_CTRL &= ~(1 << bsPWRC_EXINT_CTRL_GIO_O);
}

// Set EXINT Int Enable
void PWRC_Set_EXINT_INT_ENABLE()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->EXINT_CTRL |= 1 << bsPWRC_EXINT_CTRL_INT_EN;
}

// Set EXINT Int Disable
void PWRC_Set_EXINT_INT_DISABLE()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->EXINT_CTRL &= ~(1 << bsPWRC_EXINT_CTRL_INT_EN);
}

//--------------
// RTC_M0_HP (W0)
//
void PWRC_Clear_RTC_M0_HP()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->RTC_M0_EN = 0;
}

uint8_t PWRC_Get_RTC_M0_HP()
{
	while(RtcDomainUpdating == 1);
	return APB_PWRC->RTC_M0_HP;
}

//--------------
// RTC_M1_HP (W0)
//
void PWRC_Clear_RTC_M1_HP()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->RTC_M1_EN = 0;
}

uint8_t PWRC_Get_RTC_M1_HP()
{
	while(RtcDomainUpdating == 1);
	return APB_PWRC->RTC_M1_HP;
}

//--------------
// EXINT_HP (W0)
//
void PWRC_Clear_EXINT_HP()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->EXINT_HP = 0;
}

uint8_t PWRC_Get_EXINT_HP()
{
	while(RtcDomainUpdating == 1);
	return APB_PWRC->EXINT_HP;
}

//--------------
// EXINT_TP
//
void PWRC_Set_EXINT_TP(uint8_t tp)
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->EXINT_TP = tp;
}

uint8_t PWRC_Get_EXINT_TP()
{
  while(RtcDomainUpdating == 1);
	return APB_PWRC->EXINT_TP;
}

//-------------
// EXINT_CL (W1)
//
void PWRC_Set_EXINT_SYS_INT_CL()
{
	while(RtcDomainUpdating == 1);
	APB_PWRC->EXINT_CL = 1;
}

//-------------
// TM (RO)
//
uint8_t PWRC_Get_TM()
{
	while(RtcDomainUpdating == 1);
	return APB_PWRC->TM;
}

//-------------
// NVREG0~7
//
void PWRC_Set_NVREG(uint8_t num, uint32_t data)
{
	while(RtcDomainUpdating == 1);
	io_write(APB_PWRC_BASE + 0x70 + num*4, data);
}

uint32_t PWRC_Get_NVREG(uint8_t num)
{
	while(RtcDomainUpdating == 1);
	return io_read(APB_PWRC_BASE + 0x70 + num*4);
}

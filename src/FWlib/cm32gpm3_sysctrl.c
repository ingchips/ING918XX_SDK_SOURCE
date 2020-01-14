
#include "cm32gpm3_sysctrl.h"

//------------------------------
// SYSCTRL_ClkSwitch
//
// set
void SYSCTRL_SetSysClkSwitch(void) { AHB_SYSCTRL->SYSCTRL_SysClkSwitch = 1; }
void SYSCTRL_SetCWClkSwitch(void)  { AHB_SYSCTRL->SYSCTRL_CWClkSwitch  = 1; }
void SYSCTRL_SetApbClkSwitch(void) { AHB_SYSCTRL->SYSCTRL_ApbClkSwitch = 1; }
void SYSCTRL_SetPPSClkSwitch(void) { AHB_SYSCTRL->SYSCTRL_PPSClkSwitch = 1; }

// clear
void SYSCTRL_ClearSysClkSwitch(void) { AHB_SYSCTRL->SYSCTRL_SysClkSwitch = 0; }
void SYSCTRL_ClearCWClkSwitch(void)  { AHB_SYSCTRL->SYSCTRL_CWClkSwitch  = 0; }
void SYSCTRL_ClearApbClkSwitch(void) { AHB_SYSCTRL->SYSCTRL_ApbClkSwitch = 0; }
void SYSCTRL_ClearPPSClkSwitch(void) { AHB_SYSCTRL->SYSCTRL_PPSClkSwitch = 0; }

// get  ( Byte write, Word Read)
uint32_t SYSCTRL_GetClkSwitch(void)   { return io_read(AHB_SYSCTRL_ClkSwitch_ADDR) ; }


//-----------------------------------------
// SYSCTRL_ClkDiv
//
// set
void SYSCTRL_SetSysClkDiv(uint8_t div) { AHB_SYSCTRL->SYSCTRL_SysClkDiv = div; }
void SYSCTRL_SetCWClkDiv(uint8_t div)  { AHB_SYSCTRL->SYSCTRL_CWClkDiv  = div; }
void SYSCTRL_SetApbClkDiv(uint8_t div) { AHB_SYSCTRL->SYSCTRL_ApbClkDiv = div; }
void SYSCTRL_SetPPSClkDiv(uint8_t div) { AHB_SYSCTRL->SYSCTRL_PPSClkDiv = div; }

// get ( Byte write, Word Read)
uint32_t SYSCTRL_GetClkDiv(void)        { return io_read(AHB_SYSCTRL_ClkDiv_ADDR); }


//-----------------------------------------
// SYSCTRL_ClkGate
//
// set
void SYSCTRL_SetClkGate_AHB_DMA (void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.SET = (1 << bsSYSCTRL_ClkGate_AHB_DMA);    }
void SYSCTRL_SetClkGate_AHB_BB  (void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.SET = (1 << bsSYSCTRL_ClkGate_AHB_BB );    }
void SYSCTRL_SetClkGate_AHB_SPI0(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.SET = (1 << bsSYSCTRL_ClkGate_AHB_SPI0);   }
void SYSCTRL_SetClkGate_APB_GPIO(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.SET = (1 << bsSYSCTRL_ClkGate_APB_GPIO);   }
void SYSCTRL_SetClkGate_APB_I2C (void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.SET = (1 << bsSYSCTRL_ClkGate_APB_I2C);    }
void SYSCTRL_SetClkGate_APB_SPI1(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.SET = (1 << bsSYSCTRL_ClkGate_APB_SPI1);   }
void SYSCTRL_SetClkGate_APB_TMR0(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.SET = (1 << bsSYSCTRL_ClkGate_APB_TMR0);   }
void SYSCTRL_SetClkGate_APB_TMR1(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.SET = (1 << bsSYSCTRL_ClkGate_APB_TMR1);   }
void SYSCTRL_SetClkGate_APB_TMR2(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.SET = (1 << bsSYSCTRL_ClkGate_APB_TMR2);   }
void SYSCTRL_SetClkGate_APB_SCI0(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.SET = (1 << bsSYSCTRL_ClkGate_APB_SCI0);   }
void SYSCTRL_SetClkGate_APB_SCI1(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.SET = (1 << bsSYSCTRL_ClkGate_APB_SCI1);   }
void SYSCTRL_SetClkGate_APB_ISOL(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.SET = (1 << bsSYSCTRL_ClkGate_APB_ISOL);   }
void SYSCTRL_SetClkGate_APB_RtcClkDect(void) { AHB_SYSCTRL->SYSCTRL_ClkGate.SET = (1 << bsSYSCTRL_ClkGate_RtcClkDect); }
void SYSCTRL_SetClkGate_APB_PINC(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.SET = (1 << bsSYSCTRL_ClkGate_APB_PINC);   }


// clear
void SYSCTRL_ClearClkGate_AHB_DMA (void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.CLR = (1 << bsSYSCTRL_ClkGate_AHB_DMA);    }
void SYSCTRL_ClearClkGate_AHB_BB  (void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.CLR = (1 << bsSYSCTRL_ClkGate_AHB_BB );    } 
void SYSCTRL_ClearClkGate_AHB_SPI0(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.CLR = (1 << bsSYSCTRL_ClkGate_AHB_SPI0);   } 
void SYSCTRL_ClearClkGate_APB_GPIO(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.CLR = (1 << bsSYSCTRL_ClkGate_APB_GPIO);   } 
void SYSCTRL_ClearClkGate_APB_I2C (void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.CLR = (1 << bsSYSCTRL_ClkGate_APB_I2C);    } 
void SYSCTRL_ClearClkGate_APB_SPI1(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.CLR = (1 << bsSYSCTRL_ClkGate_APB_SPI1);   } 
void SYSCTRL_ClearClkGate_APB_TMR0(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.CLR = (1 << bsSYSCTRL_ClkGate_APB_TMR0);   } 
void SYSCTRL_ClearClkGate_APB_TMR1(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.CLR = (1 << bsSYSCTRL_ClkGate_APB_TMR1);   }
void SYSCTRL_ClearClkGate_APB_TMR2(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.CLR = (1 << bsSYSCTRL_ClkGate_APB_TMR2);   }
void SYSCTRL_ClearClkGate_APB_SCI0(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.CLR = (1 << bsSYSCTRL_ClkGate_APB_SCI0);   }
void SYSCTRL_ClearClkGate_APB_SCI1(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.CLR = (1 << bsSYSCTRL_ClkGate_APB_SCI1);   }
void SYSCTRL_ClearClkGate_APB_ISOL(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.CLR = (1 << bsSYSCTRL_ClkGate_APB_ISOL);   }
void SYSCTRL_ClearClkGate_APB_RtcClkDect(void) { AHB_SYSCTRL->SYSCTRL_ClkGate.CLR = (1 << bsSYSCTRL_ClkGate_RtcClkDect); }
void SYSCTRL_ClearClkGate_APB_PINC(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.CLR = (1 << bsSYSCTRL_ClkGate_APB_PINC);   }


// toggle 
void SYSCTRL_ToggleClkGate_AHB_DMA (void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.MODIFY = (1 << bsSYSCTRL_ClkGate_AHB_DMA);    }
void SYSCTRL_ToggleClkGate_AHB_BB  (void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.MODIFY = (1 << bsSYSCTRL_ClkGate_AHB_BB );    }
void SYSCTRL_ToggleClkGate_AHB_SPI0(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.MODIFY = (1 << bsSYSCTRL_ClkGate_AHB_SPI0);   } 
void SYSCTRL_ToggleClkGate_APB_GPIO(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.MODIFY = (1 << bsSYSCTRL_ClkGate_APB_GPIO);   } 
void SYSCTRL_ToggleClkGate_APB_I2C (void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.MODIFY = (1 << bsSYSCTRL_ClkGate_APB_I2C);    }
void SYSCTRL_ToggleClkGate_APB_SPI1(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.MODIFY = (1 << bsSYSCTRL_ClkGate_APB_SPI1);   } 
void SYSCTRL_ToggleClkGate_APB_TMR0(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.MODIFY = (1 << bsSYSCTRL_ClkGate_APB_TMR0);   } 
void SYSCTRL_ToggleClkGate_APB_TMR1(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.MODIFY = (1 << bsSYSCTRL_ClkGate_APB_TMR1);   } 
void SYSCTRL_ToggleClkGate_APB_TMR2(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.MODIFY = (1 << bsSYSCTRL_ClkGate_APB_TMR2);   } 
void SYSCTRL_ToggleClkGate_APB_SCI0(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.MODIFY = (1 << bsSYSCTRL_ClkGate_APB_SCI0);   } 
void SYSCTRL_ToggleClkGate_APB_SCI1(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.MODIFY = (1 << bsSYSCTRL_ClkGate_APB_SCI1);   } 
void SYSCTRL_ToggleClkGate_APB_ISOL(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.MODIFY = (1 << bsSYSCTRL_ClkGate_APB_ISOL);   }
void SYSCTRL_ToggleClkGate_APB_RtcClkDect(void) { AHB_SYSCTRL->SYSCTRL_ClkGate.MODIFY = (1 << bsSYSCTRL_ClkGate_RtcClkDect); }
void SYSCTRL_ToggleClkGate_APB_PINC(void)       { AHB_SYSCTRL->SYSCTRL_ClkGate.MODIFY = (1 << bsSYSCTRL_ClkGate_APB_PINC);   }


// write
void SYSCTRL_WriteClkGate(uint32_t data)  { AHB_SYSCTRL->SYSCTRL_ClkGate.WRITE = data; }

// read 
uint32_t SYSCTRL_ReadClkGate(void)  { return AHB_SYSCTRL->SYSCTRL_ClkGate.WRITE; }


//-----------------------------------------
// SYSCTRL_BlockRst
//
// set
void SYSCTRL_SetBlockRst_AHB_DMA (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_AHB_DMA);  } 
void SYSCTRL_SetBlockRst_AHB_BB  (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_AHB_BB );  }
void SYSCTRL_SetBlockRst_CW_BB   (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_CW_BB );   }
void SYSCTRL_SetBlockRst_PPS_BB  (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_PPS_BB );  }
void SYSCTRL_SetBlockRst_RTC_BB  (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_RTC_BB );  }
void SYSCTRL_SetBlockRst_RF_BB   (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_RF_BB );   }
void SYSCTRL_SetBlockRst_AHB_SPI0(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_AHB_SPI0); }
void SYSCTRL_SetBlockRst_APB_GPIO(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_APB_GPIO); }
void SYSCTRL_SetBlockRst_APB_I2C (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_APB_I2C);  }
void SYSCTRL_SetBlockRst_APB_SPI1(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_APB_SPI1); }
void SYSCTRL_SetBlockRst_APB_TMR0(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_APB_TMR0); }
void SYSCTRL_SetBlockRst_APB_TMR1(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_APB_TMR1); } 
void SYSCTRL_SetBlockRst_APB_TMR2(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_APB_TMR2); } 
void SYSCTRL_SetBlockRst_APB_SCI0(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_APB_SCI0); } 
void SYSCTRL_SetBlockRst_APB_SCI1(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_APB_SCI1); } 
void SYSCTRL_SetBlockRst_APB_ISOL(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_APB_ISOL); } 
void SYSCTRL_SetBlockRst_APB_PINC(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.SET = (1 << bsSYSCTRL_BlockRst_APB_PINC);      }


// clear
void SYSCTRL_ClearBlockRst_AHB_DMA (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_AHB_DMA);  } 
void SYSCTRL_ClearBlockRst_AHB_BB  (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_AHB_BB );  }
void SYSCTRL_ClearBlockRst_CW_BB   (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_CW_BB );   }
void SYSCTRL_ClearBlockRst_PPS_BB  (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_PPS_BB );  }
void SYSCTRL_ClearBlockRst_RTC_BB  (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_RTC_BB );  }
void SYSCTRL_ClearBlockRst_RF_BB   (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_RF_BB );   }
void SYSCTRL_ClearBlockRst_AHB_SPI0(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_AHB_SPI0); }  
void SYSCTRL_ClearBlockRst_APB_GPIO(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_APB_GPIO); }  
void SYSCTRL_ClearBlockRst_APB_I2C (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_APB_I2C);  }
void SYSCTRL_ClearBlockRst_APB_SPI1(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_APB_SPI1); }  
void SYSCTRL_ClearBlockRst_APB_TMR0(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_APB_TMR0); }  
void SYSCTRL_ClearBlockRst_APB_TMR1(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_APB_TMR1); }  
void SYSCTRL_ClearBlockRst_APB_TMR2(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_APB_TMR2); }  
void SYSCTRL_ClearBlockRst_APB_SCI0(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_APB_SCI0); }  
void SYSCTRL_ClearBlockRst_APB_SCI1(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_APB_SCI1); }  
void SYSCTRL_ClearBlockRst_APB_ISOL(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_APB_ISOL); }  
void SYSCTRL_ClearBlockRst_APB_PINC(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.CLR = (1 << bsSYSCTRL_BlockRst_APB_PINC);      }


// toggle 
void SYSCTRL_ToggleBlockRst_AHB_DMA (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_AHB_DMA);  } 
void SYSCTRL_ToggleBlockRst_AHB_BB  (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_AHB_BB );  }
void SYSCTRL_ToggleBlockRst_CW_BB   (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_CW_BB );   }
void SYSCTRL_ToggleBlockRst_PPS_BB  (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_PPS_BB );  }
void SYSCTRL_ToggleBlockRst_RTC_BB  (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_RTC_BB );  }
void SYSCTRL_ToggleBlockRst_RF_BB   (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_RF_BB );   }
void SYSCTRL_ToggleBlockRst_AHB_SPI0(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_AHB_SPI0); }  
void SYSCTRL_ToggleBlockRst_APB_GPIO(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_APB_GPIO); }  
void SYSCTRL_ToggleBlockRst_APB_I2C (void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_APB_I2C);  }
void SYSCTRL_ToggleBlockRst_APB_SPI1(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_APB_SPI1); }  
void SYSCTRL_ToggleBlockRst_APB_TMR0(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_APB_TMR0); }  
void SYSCTRL_ToggleBlockRst_APB_TMR1(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_APB_TMR1); }  
void SYSCTRL_ToggleBlockRst_APB_TMR2(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_APB_TMR2); }  
void SYSCTRL_ToggleBlockRst_APB_SCI0(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_APB_SCI0); }  
void SYSCTRL_ToggleBlockRst_APB_SCI1(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_APB_SCI1); }  
void SYSCTRL_ToggleBlockRst_APB_ISOL(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_APB_ISOL); }  
void SYSCTRL_ToggleBlockRst_APB_PINC(void) { AHB_SYSCTRL->SYSCTRL_BlockRst.MODIFY = (1 << bsSYSCTRL_BlockRst_APB_PINC); }


// write
void SYSCTRL_WriteBlockRst(uint32_t data)  { AHB_SYSCTRL->SYSCTRL_BlockRst.WRITE = data; }

// read 
uint32_t SYSCTRL_ReadBlockRst(void)  { return AHB_SYSCTRL->SYSCTRL_BlockRst.WRITE; }


//-----------------------------------------
// SYSCTRL_SoftwareRst
//
// set 
void SYSCTRL_SetSoftwareRst(void)  { AHB_SYSCTRL->SYSCTRL_SoftwareRst = 1; }

// get
uint8_t SYSCTRL_GetSoftwareRst(void)  { return AHB_SYSCTRL->SYSCTRL_SoftwareRst; }

//-----------------------------------------
// SYSCTRL_RtcClkCount
//
// read 
uint32_t SYSCTRL_ReadRtcClkCount(void)  { return AHB_SYSCTRL->SYSCTRL_RtcClkCount; }


//-----------------------------------------
// SYSCTRL_PLLParam
//
// set
void SYSCTRL_SetPLLONE_N(uint8_t   data) { AHB_SYSCTRL->SYSCTRL_PLLParamONE_N   = data; }
void SYSCTRL_SetPLLONE_M(uint8_t   data) { AHB_SYSCTRL->SYSCTRL_PLLParamONE_M   = data; }
void SYSCTRL_SetPLLONE_OD(uint8_t  data) { AHB_SYSCTRL->SYSCTRL_PLLParamONE_OD  = data; }

void SYSCTRL_SetPLLTWO_RST(uint8_t data) { AHB_SYSCTRL->SYSCTRL_PLLParamTWO_RST = data; }
void SYSCTRL_SetPLLTWO_BP(uint8_t  data) { AHB_SYSCTRL->SYSCTRL_PLLParamTWO_BP  = data; }


// get 
uint32_t SYSCTRL_GetPLLONE(void)          { return io_read(AHB_SYSCTRL_PLLParamONE_ADDR); }

uint32_t SYSCTRL_GetPLLTWO(void)          { return io_read(AHB_SYSCTRL_PLLParamTWO_ADDR); }

uint8_t SYSCTRL_PLL_LOCKED(void)
{
	return (SYSCTRL_GetPLLTWO() >> bsSYSCTRL_PLLParamTWO_LOCK) & BW2M(bwSYSCTRL_PLLParamTWO_LOCK);
}










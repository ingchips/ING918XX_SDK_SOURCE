#include "stdlib.h"
#include "ingsoc.h"
#include "app_cfg.h"
#include "platform_api.h"

int DMA_PreparePeripheral2Mem(DMA_Descriptor *pDesc,
                                       uint32_t *dst, SYSCTRL_DMA src, int size,
                                       DMA_AddressControl dst_addr_ctrl,
                                       uint32_t options);

#if (SAMPLING_RATE != 16000)
#error only 16kHz is supported
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

// Note: MIC is on the RIGHT channel.
// Tested on INMP441.
static uint32_t sample_counter = 1;

#define TRIGGER_NUMBER  8
#define CHANNEL_ID  0
#define PP_LEN  80
   
typedef enum {
    PING_WRITE,
    PANG_WRITE,
} DMA_PINGPANG_WRITE;

typedef struct tagDMA_PingPang {
    uint32_t ping[PP_LEN];
    uint32_t pang[PP_LEN];
    DMA_PINGPANG_WRITE status;
	DMA_Descriptor descriptor_pp[2] __attribute__((aligned (8)));
} DMA_PingPang;
DMA_PingPang PingPang;

int transWordsNum = 0;
void FindTransWordsNum(void)
{
	int i = 1;	
	while (TRIGGER_NUMBER * i <= PP_LEN) {
		i++;
	}
	transWordsNum = TRIGGER_NUMBER * (i - 1);
}

static uint32_t DMA_cb_isr(void *user_data)
{ 
	uint32_t state = DMA_GetChannelIntState(CHANNEL_ID);
	DMA_ClearChannelIntState(CHANNEL_ID, state);

	PingPang.status ^= 1;
    DMA_EnableChannel(CHANNEL_ID, &PingPang.descriptor_pp[PingPang.status]);

	uint32_t *rr = (PingPang.status == PING_WRITE) ? (PingPang.pang) : (PingPang.ping);
    int i = sample_counter;
    while (i < transWordsNum) {
        audio_rx_sample((pcm_sample_t)(rr[i] >> 8));
        i += 2;
    }
    
    return 0;
}

int FindBurstNum(void)
{
    for (int i = 0; i <= 3; ++i) {
        if ((1 << i) == TRIGGER_NUMBER) {
            return i;
        }
    }
    return -1;
}

void DMA_SetUpPingPang(void)
{
	PingPang.descriptor_pp[0].Next = &PingPang.descriptor_pp[1];
	PingPang.descriptor_pp[1].Next = &PingPang.descriptor_pp[0];
	FindTransWordsNum();
    int burstNum = FindBurstNum();
    DMA_PreparePeripheral2Mem(&PingPang.descriptor_pp[PING_WRITE], 
							  (uint32_t *)PingPang.ping, 
							  SYSCTRL_DMA_I2S_RX, 
							  transWordsNum * 4, 
							  DMA_ADDRESS_INC, 
							  0
							  | burstNum << 24
							  | 1 << DMA_DESC_OPTION_BIT_INTERRUPT_EACH_DESC);
    DMA_PreparePeripheral2Mem(&PingPang.descriptor_pp[PANG_WRITE], 
							  (uint32_t *)PingPang.pang, 
							  SYSCTRL_DMA_I2S_RX, 
							  transWordsNum * 4, 
							  DMA_ADDRESS_INC, 
							  0
							  | burstNum << 24
							  | 1 << DMA_DESC_OPTION_BIT_INTERRUPT_EACH_DESC);
    PingPang.status = PING_WRITE;
}

// static void set_reg_bits(volatile uint32_t *reg, uint32_t v=0x11, uint8_t bit_width, uint8_t bit_offset)
// {
//     uint32_t mask = ((1 << bit_width) - 1) << bit_offset;0xFFFFFFFF
//     *reg = (*reg & ~mask) | (v << bit_offset);
// }
// void PINCTRL_SetPadMux(const uint8_t io_pin_index, const io_source_t source)
// {
//     set_reg_bits(&APB_PINCTRL->OUT_CTRL[io_pin_index >> 2], source, 8, 8 * (io_pin_index & 0x3));
//     set_reg_bits(&APB_PINCTRL->OUT_CTRL[10], 0x11, 8, 16);
// }
void audio_input_setup(void)
{
   PINCTRL_SetPadMux(I2S_PIN_BCLK, IO_SOURCE_I2S_BCLK_OUT);
   PINCTRL_SetPadMux(I2S_PIN_IN, IO_SOURCE_I2S_DATA_IN);
   PINCTRL_SelI2sIn(IO_NOT_A_PIN, IO_NOT_A_PIN, I2S_PIN_IN);
   PINCTRL_SetPadMux(I2S_PIN_LRCLK, IO_SOURCE_I2S_LRCLK_OUT);
   PINCTRL_Pull(IO_SOURCE_I2S_DATA_IN, PINCTRL_PULL_DOWN);

   SYSCTRL_ConfigPLLClk(6, 128, 2);                    // PLL = 256MHz
   SYSCTRL_SelectHClk(SYSCTRL_CLK_PLL_DIV_1 + 1);      // MCU @ 128MHz
   SYSCTRL_SelectI2sClk(SYSCTRL_CLK_PLL_DIV_1 + 4);    // I2S Clk @ 51.2MHz
   I2S_ConfigClk(APB_I2S, 25, 32);                     // F_bclk = 1.024MHz
   I2S_ConfigIRQ(APB_I2S, 0, 1, 0, TRIGGER_NUMBER);
   I2S_DMAEnable(APB_I2S, 0, 0);    
   I2S_Config(APB_I2S, I2S_ROLE_MASTER, I2S_MODE_STANDARD, 0, 1, 0, 1, 24);

   DMA_SetUpPingPang();
   platform_set_irq_callback(PLATFORM_CB_IRQ_DMA, DMA_cb_isr, 0);
}

void audio_input_start(void)
{
   DMA_EnableChannel(CHANNEL_ID, &PingPang.descriptor_pp[PingPang.status]);
   I2S_ClearRxFIFO(APB_I2S);
   I2S_DMAEnable(APB_I2S, 1, 1);    
   I2S_Enable(APB_I2S, 0, 1);
}

void audio_input_stop(void)
{
   I2S_Enable(APB_I2S, 0, 0);
   I2S_DMAEnable(APB_I2S, 0, 0);  
}

#else
#error only ING916xx is supported
#endif

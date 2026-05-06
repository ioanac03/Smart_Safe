#include "header.h"

volatile bool alarma_ldr = false;

#define DMA_SAMPLES 8
static uint16_t adc_buf[DMA_SAMPLES];
static int dma_chan = -1;

void dma_irq_handler(){
    if(dma_channel_get_irq0_status(dma_chan)){
        dma_channel_acknowledge_irq0(dma_chan);

        dma_channel_set_write_addr(dma_chan, adc_buf, true);
    }
}

void ldr_init(){
    //ADC
    adc_init();
    adc_gpio_init(LDR_PIN);
    adc_select_input(LDR_ADC_CH);

    //free-running ADC
    adc_fifo_setup(
        true,    
        true,    //enable DMA request
        1,  
        false,   
        false     
    );
    adc_set_clkdiv(0); //max speed

    //DMA
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);

    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    channel_config_set_dreq(&cfg, DREQ_ADC);

    dma_channel_configure(
        dma_chan,
        &cfg,
        adc_buf,   // dst
        &adc_hw->fifo,      // src 
        DMA_SAMPLES,
        false       // nu porni inca
    );

    //intrerupere DMA la final transfer
    dma_channel_set_irq0_enabled(dma_chan, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    //pornim ADC + DMA
    dma_channel_start(dma_chan);
    adc_run(true);
}

uint16_t ldr_read(){
    uint32_t sum = 0;
    for(int i = 0; i < DMA_SAMPLES; i++){
        sum += adc_buf[i];
    }

    return (uint16_t)(sum / DMA_SAMPLES);
}

bool ldr_lumina_detectata(){
    uint16_t val = ldr_read();
    printf("LDR ADC: %d\n", val);
    return val > LDR_THRESHOLD;
}
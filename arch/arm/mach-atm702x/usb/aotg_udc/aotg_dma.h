#ifndef  __LINUX_USB_AOTG_DMA_H__ 
#define  __LINUX_USB_AOTG_DMA_H__ 

typedef void (* aotg_dma_handler)(void *data);

int aotg_dma_reset(int dma_nr);
int aotg_dma_enable_irq(int dma_nr);
int aotg_dma_is_irq(int dma_nr);

int aotg_dma_clear_pend(int dma_nr);

unsigned int aotg_dma_get_cmd(unsigned int dma_nr);
unsigned int aotg_dma_get_cnt(int dma_nr);
unsigned int aotg_dma_get_memaddr(int dma_nr);
int aotg_dma_stop(int dma_nr);
unsigned int aotg_dma_get_remain(int dma_nr);
//
int aotg_dma_set_mode(int dma_nr, unsigned char ep_select);
int aotg_dma_set_memaddr(int dma_nr, unsigned long addr);
int aotg_dma_set_cnt(int dma_nr, unsigned long dma_length);
int aotg_dma_start(int dma_nr);

/* discard functions. */
//int disable_dma_htcirq(int dma_nr);
//int clear_dma_htcirq_pend(int dma_nr)
//int pause_dma_for_usb(int dma_nr);
//int get_dma_mode(int dma_nr);

int aotg_dma_request(void *data);
void aotg_dma_free(int dma_nr);
unsigned char aotg_dma_is_finish(int dma_nr);
void aotg_dma_wait_finish(int dma_nr);

#endif /* __LINUX_USB_AOTG_DMA_H__ */ 
 

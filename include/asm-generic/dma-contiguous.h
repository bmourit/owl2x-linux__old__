#ifndef ASM_DMA_CONTIGUOUS_H
#define ASM_DMA_CONTIGUOUS_H

#ifdef __KERNEL__
#ifdef CONFIG_CMA

#include <linux/device.h>
#include <linux/dma-contiguous.h>

static inline struct cma *dev_get_cma_area(struct device *dev)
{
	if(!dev)
		printk("dma_contiguous_default_area: 0x%08x\n", dma_contiguous_default_area);		
	if (dev && dev->cma_area)
		return dev->cma_area;
	return dma_contiguous_default_area;
}

static inline void dev_set_cma_area(struct device *dev, struct cma *cma)
{
	if (dev)
		dev->cma_area = cma;
	if (!dev || !dma_contiguous_default_area)
		dma_contiguous_default_area = cma;
	printk("dev: 0x%08x\n", dev);
	printk("dma_contiguous_default_area: 0x%08x\n", dma_contiguous_default_area);		
}

#endif
#endif

#endif

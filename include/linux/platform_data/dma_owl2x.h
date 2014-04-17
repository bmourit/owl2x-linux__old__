/*
 * arch/arm/mach-leopard/include/mach/dma.h
 *
 * dma interface
 *
 * Copyright 2012 Actions Semi Inc.
 * Author: Actions Semi, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __ASM_ACT213X_DMA_H
#define __ASM_ACT213X_DMA_H

#include <linux/delay.h>
#include <mach/hardware.h>
#include <mach/clock.h>
#include <mach/debug.h>

#if defined(CONFIG_ARCH_LEOPARD)
#define NUM_DMA_CHANNELS                4
#define DMA_CHANNEL_LEN                 0x00000030
#else
#error "No DMA_CHANNEL_LEN is defined"
#endif

#define NUM_ASOC_DMA_CHANNELS           NUM_DMA_CHANNELS

/* DMA Channel Base Addresses */
#define DMA_CHANNEL_BASE                IO_ADDRESS(BDMA0_BASE)

/* register offset */
#define DMA_MODE                        0x0000
#define DMA_SRC                         0x0004
#define DMA_DST                         0x0008
#define DMA_CNT                         0x000C
#define DMA_REM                         0x0010
#define DMA_CMD                         0x0014
#define DMA_CACHE                       0x0018

#define DMA_START                       0x01
#define DMA_COUNT_MASK                  0x00ffffff

#define DMA_IRQPD_DXTP(x)               (1 << ((x) * 2))

#define DMA_CACHE_USERS_STRONG_ORDERED  (0x0)
#define DMA_CACHE_USERS_DEVICE          (0x1)
#define DMA_CACHE_USERS_UNCACHEABLE     (0x3)
#define DMA_CACHE_USERS_WRITE_THROUGH   (0x6)
#define DMA_CACHE_USERS_WRITEBACK_NOALLOC   (0x7)
#define DMA_CACHE_USERS_WRITEBACK_ALLOC (0xf)
#define DMA_CACHE_USERS_SHARED          (1)

#define DMA_CACHE_WBA_S                 (0xf1f)

#ifndef _ASSEMBLER_

enum {
    DMA_CHAN_TYPE_BUS
};


typedef void (*chan_callback_t)(unsigned int ,void *);

/**
 * struct dma_chan - dma channel struct
 * @is_used: register address
 * @io: register value
 * @dev_str: the char pointer points to interrupt handler name
 * @irq: irq number
 * @irq_dev: irq
 * @chan_type: channel type
 * @callback: callback function
 *
 * dma_chan struct definition
 */
struct dma_chan {
    int is_used;        /*this channel is allocated if !=0, free if ==0*/
    unsigned int io;
    const char *dev_str;
    int irq;
    void *irq_dev;
    unsigned int chan_type;
    chan_callback_t callback;       /*callback to this channel*/
};

/* These are in arch/mips/atj213x/common/dma.c */
extern struct dma_chan asoc_dma_table[];
extern spinlock_t dma_regop_lock;

//typedef void (*irqhandler_t)(unsigned int,void *,struct pt_regs *);
typedef void (*irqhandler_t)(int,void *);


void asoc_dma_irq_init(void);

/**
 * request_act213x_dma() - request the dma channel
 * @chan_type:  dma channel type {DMA_CHAN_TYPE_SPECIAL,DMA_CHAN_TYPE_BUS}
 * @dev_str:    Describe the second argument to foobar.
 * @irqhandler: the pointer to the dma interrupt handler.
 * @irqflags:   Describe the second argument to foobar.
 * @irq_dev_id: Describe the second argument to foobar.
 *      One can provide multiple line descriptions
 *      for arguments.
 *
 * function request_act213x_dma() is used to request the dma channel.
 *
 * Returns -1 if no dma channel is available,
 * otherwise return the dma channel number
 **/
int request_asoc_dma(unsigned int chan_type, const char *dev_str, irqhandler_t irqhandler,
                        unsigned long irqflags, void *irq_dev_id);

/**
 * free_act213x_dma() - free the dma channel
 * @dmanr:  dma channel number
 *
 * A longer description, with more discussion of the function foobar()
 * that might be useful to those using or modifying it.  Begins with
 * empty comment line, and may include additional embedded empty
 * comment lines.
 *
 * The longer description can have multiple paragraphs.
 **/
void free_asoc_dma(unsigned int dmanr);

struct dma_chan *get_dma_chan(unsigned int dmanr);

static __inline__ void set_dma_cpu_priority(unsigned int prio)
{
    act_writel(prio&0x0000000f, DMA_CTL);
}

static __inline__ int get_dma_cpu_priority(void)
{
    return (act_readl(DMA_CTL) & 0x0000000f);
}

/**
 * reset_dma() - reset the dma channel
 * @dmanr:  dma channel number
 *
 * This function is used to reset the dma channel
 **/

static __inline__ void reset_dma(unsigned int dmanr)
{
}


/**
 * pause_dma() - pause the dma transfer
 * @dmanr:  dma channel number
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ void pause_dma(unsigned int dmanr)
{
    struct dma_chan *chan = get_dma_chan(dmanr);
    unsigned long flag;

    if (!chan)
        return;

    spin_lock_irqsave(&dma_regop_lock, flag);
    act_writel((1 << dmanr) | act_readl(DMA_PAUSE), DMA_PAUSE);
    spin_unlock_irqrestore(&dma_regop_lock, flag);
}

/**
 * resume_dma() - resume the dma transfer
 * @dmanr:  dma channel number
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ void resume_dma(unsigned int dmanr)
{
    struct dma_chan *chan = get_dma_chan(dmanr);
    unsigned long flag;

    if (!chan)
        return;

    spin_lock_irqsave(&dma_regop_lock, flag);
    act_writel( (~(1 << dmanr)) & act_readl(DMA_PAUSE), DMA_PAUSE);
    spin_unlock_irqrestore(&dma_regop_lock, flag);
}

/**
 * dma_paused() - check if the dma transfer is paused
 * @dmanr:  dma channel number
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ int dma_paused(unsigned int dmanr)
{
    struct dma_chan *chan = get_dma_chan(dmanr);

    if (!chan)
        return 1;

    return (act_readl(DMA_PAUSE) & (1 << dmanr)) ? 1 : 0;
}

/**
 * start_dma() - start the dma transfer
 * @dmanr:  dma channel number
 *
 * This function is used to start the dma transfer
 **/
static __inline__ void start_dma(unsigned int dmanr)
{
    struct dma_chan *chan = get_dma_chan(dmanr);

    if (!chan)
        return;

    resume_dma(dmanr);

    /*
     * Shareable cacheable write data may not automatically become visible to ACP
     */
    isb();
    dsb();

    act_writel(DMA_START, chan->io + DMA_CMD);
}

/**
 * dma_started() - test if the dma channel started
 * @dmanr:  dma channel number
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ int dma_started(unsigned int dmanr)
{
    struct dma_chan *chan = get_dma_chan(dmanr);

    if (!chan)
        return 0;

    return (act_readl(chan->io + DMA_CMD) & DMA_START) ? 1 : 0;
}

static __inline__ void stop_dma_by_ddr2ddr_mode(unsigned int dmanr)
{
    struct dma_chan *chan = get_dma_chan(dmanr);
    unsigned long flag;
    unsigned int mode, mode_bak;

    if (!chan)
        return;

    printk("[DMA%d] %s: before rescue, DMA_MODE 0x%x, DMA_CMD 0x%x\n",
        dmanr, __FUNCTION__,
        act_readl(chan->io + DMA_MODE),
        act_readl(chan->io + DMA_CMD));

    spin_lock_irqsave(&dma_regop_lock, flag);

    mode_bak = act_readl(chan->io + DMA_MODE);
    mode = mode_bak;
    mode &= ~0x1f001f;
    mode |= 0x120012;
    act_writel(mode, chan->io + DMA_MODE);

    act_writel((~DMA_START) & act_readl(chan->io + DMA_CMD),
        chan->io + DMA_CMD);

    udelay(1);

    act_writel(mode_bak, chan->io + DMA_MODE);

    spin_unlock_irqrestore(&dma_regop_lock, flag);

    printk("[DMA%d] %s: after rescue, DMA_MODE 0x%x, DMA_CMD 0x%x\n",
        dmanr, __FUNCTION__,
        act_readl(chan->io + DMA_MODE),
        act_readl(chan->io + DMA_CMD));
}


/**
 * wait_dma_stopped() - wait for the dma transfer completion
 * @dmanr:  dma channel number
 * @timeout_us:  timeout (us)
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ void wait_dma_stopped(unsigned int dmanr, int timeout_us)
{
    struct dma_chan *chan = get_dma_chan(dmanr);

    if (!chan)
        return;

    while (timeout_us > 0 && (act_readl(chan->io + DMA_CMD) & DMA_START) != 0) {
        udelay(1);
        timeout_us -= 1;
    }

    if (timeout_us <= 0) {
        pr_err("[DMA] error: wait dma%d stopped timeout!!!\n", dmanr);

        /* rescue the DMA DRQ by ddr2ddr mode */
        stop_dma_by_ddr2ddr_mode(dmanr);
    }
}


/**
 * stop_dma() - stop the dma transfer
 * @dmanr:  dma channel number
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ void stop_dma(unsigned int dmanr)
{
    struct dma_chan *chan = get_dma_chan(dmanr);
    unsigned long flag;

    if (!chan)
        return;

    pause_dma(dmanr);

    spin_lock_irqsave(&dma_regop_lock, flag);
    act_writel((~DMA_START) & act_readl(chan->io + DMA_CMD), 
        chan->io + DMA_CMD);
    spin_unlock_irqrestore(&dma_regop_lock, flag);

    /* wait timeout: 1000us */
    wait_dma_stopped(dmanr, 1000);
}


/**
 * set_dma_mode() - set the dma transfer mode
 * @dmanr:  dma channel number
 * @mode: dma transfer mode
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ void set_dma_mode(unsigned int dmanr, unsigned int mode)
{
    struct dma_chan *chan = get_dma_chan(dmanr);

    if (!chan)
        return;

    act_writel(mode, chan->io + DMA_MODE);
}

/**
 * set_dma_cache_mode() - set the dma cache mode
 * @dmanr:  dma channel number
 * @mode: dma cache mode
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ void set_dma_cache_mode(unsigned int dmanr, unsigned int mode)
{
    struct dma_chan *chan = get_dma_chan(dmanr);

    if (!chan)
        return;

    act_writel(mode, chan->io + DMA_CACHE);
}

/**
 * get_dma_mode() - get the dma transfer mode
 * @dmanr:  dma channel number
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ unsigned int get_dma_mode(unsigned int dmanr)
{
    struct dma_chan *chan = get_dma_chan(dmanr);

    if (!chan)
        return 0;

    return act_readl(chan->io + DMA_MODE);
}

/**
 * set_dma_src_addr() - set the dma transfer source address
 * @dmanr:  dma channel number
 * @a: source address
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ void set_dma_src_addr(unsigned int dmanr, unsigned int a)
{
    struct dma_chan *chan = get_dma_chan(dmanr);

    if (!chan)
        return;

    act_writel(a, chan->io + DMA_SRC);
}

/**
 * set_dma_dst_addr() - set the dma transfer destination address
 * @dmanr:  dma channel number
 * @a: destination address
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ void set_dma_dst_addr(unsigned int dmanr, unsigned int a)
{
    struct dma_chan *chan = get_dma_chan(dmanr);

    if (!chan)
        return;

    act_writel(a, chan->io + DMA_DST);
}

/**
 * set_dma_count() - set the dma transfer length
 * @dmanr:  dma channel number
 * @count: dma transfer byte counts
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ void set_dma_count(unsigned int dmanr, unsigned int count)
{
    struct dma_chan *chan = get_dma_chan(dmanr);

    if (!chan)
        return;

    count &= DMA_COUNT_MASK;
    act_writel(count, chan->io + DMA_CNT);
}

static __inline__ int get_dma_count(unsigned int dmanr)
{
    int count;
    struct dma_chan *chan = get_dma_chan(dmanr);

    if (!chan)
        return -1;

    count = act_readl(chan->io + DMA_CNT) & DMA_COUNT_MASK;

    return count;
}

/**
 * get_dma_remain() - get the remained transfer length
 * @dmanr:  dma channel number
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ int get_dma_remain(unsigned int dmanr)
{
    int count;
    struct dma_chan *chan = get_dma_chan(dmanr);

    if (!chan)
        return -1;

    count = act_readl(chan->io + DMA_REM) & DMA_COUNT_MASK;

    return count;
}

/**
 * clear_dma_remain() - short function description of foobar
 * @dmanr:  dma channel number
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ int clear_dma_remain(unsigned int dmanr)
{
    struct dma_chan *chan = get_dma_chan(dmanr);

    if (!chan)
        return -1;

    act_writel(0, chan->io + DMA_REM);
    return 0;
}

/**
 * get_dma_tcirq_pend() - short function description of foobar
 * @dmanr:  dma channel number
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ int get_dma_tcirq_pend(unsigned int dmanr)
{
    struct dma_chan *chan = get_dma_chan(dmanr);

    if (!chan)
        return -1;

    return (act_readl(DMA_IRQPD) & (1<<(dmanr*2))) ? 1 : 0;
}

/**
 * clear_dma_tcirq_pend() - short function description of foobar
 * @dmanr:  dma channel number
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ int clear_dma_tcirq_pend(unsigned int dmanr)
{
    struct dma_chan *chan = get_dma_chan(dmanr);

    if (!chan)
        return -1;
    else
        act_writel(1<<(dmanr*2), DMA_IRQPD);

    return 0;
}

/**
 * enable_dma_tcirq() - short function description of foobar
 * @dmanr:  dma channel number
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ int enable_dma_tcirq(unsigned int dmanr)
{
    struct dma_chan *chan = get_dma_chan(dmanr);
    unsigned long flag;

    if (!chan)
        return -1;

    spin_lock_irqsave(&dma_regop_lock, flag);
    act_writel(act_readl(DMA_IRQEN)|(1<<(dmanr*2)), DMA_IRQEN);
    spin_unlock_irqrestore(&dma_regop_lock, flag);

    return 0;
}

/**
 * disable_dma_tcirq() - short function description of foobar
 * @dmanr:  dma channel number
 *
 * The longer description can have multiple paragraphs.
 **/
static __inline__ int disable_dma_tcirq(unsigned int dmanr)
{
    struct dma_chan *chan = get_dma_chan(dmanr);
    unsigned long flag;

    if (!chan)
        return -1;

    spin_lock_irqsave(&dma_regop_lock, flag);
    act_writel(act_readl(DMA_IRQEN)&(~(1<<(dmanr*2))), DMA_IRQEN);
    spin_unlock_irqrestore(&dma_regop_lock, flag);

    return 0;
}

#endif /*_ASSEMBLER_*/
#endif /* __ASM_ACT213X_DMA_H */

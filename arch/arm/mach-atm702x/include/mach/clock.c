/*  
 *  linux/arch/arm/mach-atm702x/clock.c
 *
 *  Copyright (C) 2014
 *  Author: B. Mouritsen <bnmguy@gmail.com>
 *
 *  This program is free software; you can redistribute	 it and/or modify it
 *  under  the terms of	 the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the	License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/clk.h>
#include <linux/io.h>

#include <mach/apb.h>
#include <mach/clock.h>

static void apb_clk_enable(struct clk *clk)
{
    uint32_t clk_rst;

    clk_rst = APB_APBCLK | APB_FNCLK | APB_FNCLKSEL(clk->fnclksel);
    __raw_writel(clk_rst, clk->clk_rst);
}

static void apb_clk_disable(struct clk *clk)
{
    __raw_writel(0, clk->clk_rst);
}

struct clkops apb_clk_ops = {
    .enable     = apb_clk_enable,
    .disable    = apb_clk_disable,
};

static DEFINE_SPINLOCK(clocks_lock);

int clk_enable(struct clk *clk)
{
        unsigned long flags;

        spin_lock_irqsave(&clocks_lock, flags);
        if (clk->enabled++ == 0)
            clk->ops->enable(clk);
        spin_unlock_irqrestore(&clocks_lock, flags);
        return 0;
}
EXPORT_SYMBOL(clk_enable);

void clk_disable(struct clk *clk)
{
        unsigned long flags;

        WARN_ON(clk->enabled == 0);

        spin_lock_irqsave(&clocks_lock, flags);
        if (--clk->enabled == 0)
            clk->ops->disable(clk);
        sping_unlock_irqrestore(&clocks_lock, flags);
}
EXPORT_SYMBOL(clk_disable);

unsigned long clk_get_rate(struct clk *clk)
{
        unsigned long rate;

        if (clk->ops->getrate)
            rate = clk->ops->getrate(clk);
        else
            rate = clk->rate;

        return rate;
}
EXPORT_SYMBOL(clk_get_rate);

int clk_set_rate(struct clk *clk, unsigned long rate)
{
        unsigned long flags;
        int ret = -EINVAL;

        if (clk->ops->setrate) {
                spin_lock_irqsave(&clocks_lock, flags);
                ret = clk->ops->setrate(clk, rate);
                spin_unlock_irqrestore(&clocks_lock, flags);
        }

        return ret;
}
EXPORT_SYMBOL(clk_set_rate);

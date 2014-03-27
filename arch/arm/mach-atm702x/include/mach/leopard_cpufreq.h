#ifndef __ARM_LEOPARD_CPUFREQ_H__
#define __ARM_LEOPARD_CPUFREQ_H__

enum {
  CLR_VDD_LIMIT=0,  
  SET_VDD_LIMIT,
};
int vdd_voltage_limit_control(unsigned int cmd, unsigned int vdd);
#endif
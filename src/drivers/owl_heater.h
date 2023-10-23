#ifndef OWL_HEATER_H
#define OWL_HEATER_H

#define N_STBS 6

void heater_init();

void heater_run_stb(int id);

void heater_stop_stbs();

void heater_power_on();

void heater_power_off();

#endif
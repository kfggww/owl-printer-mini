#ifndef OWL_MOTOR_H
#define OWL_MOTOR_H

void motor_init();

void motor_run(int nstep);

void motor_reverse_run(int nstep);

void motor_stop();
#endif
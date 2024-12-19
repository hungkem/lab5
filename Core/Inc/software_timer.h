/*
 * software_timer.h
 *
 *  Created on: Dec 10, 2024
 *      Author: PC
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#define max 10
#define tick 10
extern int timer_flag[max];
void set_timer(int index, int duration);
int is_time_expired(int index);
void timer_run();

#endif /* INC_SOFTWARE_TIMER_H_ */

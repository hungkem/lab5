/*
 * software_timer.c
 *
 *  Created on: Dec 10, 2024
 *      Author: PC
 */
#include "software_timer.h"

int timer_counter[max];
int timer_flag[max];
void set_timer(int index, int duration)
{
	timer_counter[index] = duration/tick;
	timer_flag[index] = 0;
}

int is_time_expired(int index)
{
	if(timer_flag[index] == 1)
	{
		timer_flag[index] = 0;
		return 1;
	}
	return 0;
}

void timer_run()
{
	for(int i = 0; i < max; i++)
	{
		if(timer_counter[i] > 0)
		{
			timer_counter[i]--;
		}
		if(!timer_counter[i])
		{
			timer_flag[i] = 1;
		}
	}
}

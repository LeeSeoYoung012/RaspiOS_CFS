#include "utils.h"
#include "printf.h"
#include "sched.h"
#include "peripherals/timer.h"
#include "timer.h"

const unsigned int interval = 200000;
unsigned int curVal = 0;

void timer_init ( void )
{
	curVal = get32(TIMER_CLO);
	curVal += interval;
	put32(TIMER_C1, curVal);
}

void handle_timer_irq( void ) 
{
	curVal += interval;
	put32(TIMER_C1, curVal);
	put32(TIMER_CS, TIMER_CS_M1);
	timer_tick();
}


void generic_timer_init ( void ) //physical timer 실행과 동시에 리셋
{
	gen_timer_init(); //타이머 초기화
	gen_timer_reset(); //compare register 업데이트
}

void handle_generic_timer_irq( void ) //리셋을 함.
{
	printf("Timer interrupt received\n\r");
	gen_timer_reset();//timer interrupt 다시 발생하면서 interval timer reset
	timer_tick();//스케줄링을 한다.
}

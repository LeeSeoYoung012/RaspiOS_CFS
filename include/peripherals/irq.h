#ifndef	_P_IRQ_H
#define	_P_IRQ_H

#include "peripherals/base.h"

#define IRQ_BASIC_PENDING	(PBASE+0x0000B200)
#define IRQ_PENDING_1		(PBASE+0x0000B204)
#define IRQ_PENDING_2		(PBASE+0x0000B208)
#define FIQ_CONTROL		(PBASE+0x0000B20C)
#define ENABLE_IRQS_1		(PBASE+0x0000B210)
#define ENABLE_IRQS_2		(PBASE+0x0000B214)
#define ENABLE_BASIC_IRQS	(PBASE+0x0000B218)
#define DISABLE_IRQS_1		(PBASE+0x0000B21C)
#define DISABLE_IRQS_2		(PBASE+0x0000B220)
#define DISABLE_BASIC_IRQS	(PBASE+0x0000B224)

#define SYSTEM_TIMER_IRQ_0	(1 << 0)
#define SYSTEM_TIMER_IRQ_1	(1 << 1)
#define SYSTEM_TIMER_IRQ_2	(1 << 2)
#define SYSTEM_TIMER_IRQ_3	(1 << 3)

#define TIMER_INT_CTRL_0 (0x40000040) //core 0의 timer interrupt control 주소
#define INT_SOURCE_0     (LPBASE+0x60) //core 0 번에 interrupt 발생시 pending bit를 set,interrupt 발생시 1번 bit로
                                       // CNTPNSIRQ 확인 가능

#define TIMER_INT_CTRL_0_VALUE (1<<1)    //interrupt controller 주소로 접근해서 controller 가능하게함
#define GENERIC_TIMER_INTERRUPT (1<<1)   //CNTPNSIRQ 가 1로 셋팅

#endif  /*_P_IRQ_H */

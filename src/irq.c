#include "utils.h"
#include "printf.h"
#include "timer.h"
#include "entry.h"
#include "peripherals/irq.h"

const char *entry_error_messages[] = {
	"SYNC_INVALID_EL1t",
	"IRQ_INVALID_EL1t",		
	"FIQ_INVALID_EL1t",		
	"ERROR_INVALID_EL1T",		

	"SYNC_INVALID_EL1h",		
	"IRQ_INVALID_EL1h",		
	"FIQ_INVALID_EL1h",		
	"ERROR_INVALID_EL1h",		

	"SYNC_INVALID_EL0_64",		
	"IRQ_INVALID_EL0_64",		
	"FIQ_INVALID_EL0_64",		
	"ERROR_INVALID_EL0_64",	

	"SYNC_INVALID_EL0_32",		
	"IRQ_INVALID_EL0_32",		
	"FIQ_INVALID_EL0_32",		
	"ERROR_INVALID_EL0_32"	
};

void enable_interrupt_controller()
{
	put32(TIMER_INT_CTRL_0, TIMER_INT_CTRL_0_VALUE);
}

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address)
{
	printf("%s, ESR: %x, address: %x\r\n", entry_error_messages[type], esr, address);
}

void handle_irq(void)
{	
	unsigned int irq = get32(INT_SOURCE_0);//interrupt 발생시 pending bit가 set 됨 즉 pending bit 확인하기위해 이주소에 있는 레지스터값을 가져옴
	switch (irq) {
		case (GENERIC_TIMER_INTERRUPT):// CNTPNSIRQ가 발생했을 경우
			handle_generic_timer_irq(); //timer_handle을 하게된다. 
			break;
		default:
			printf("Unknown pending irq: %x\r\n", irq);
	}
}


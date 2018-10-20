////////////////////TODO: empty exception handler, initialization, CALL x86desc SET_IDT_ENTRY, assmebly, call init in kernal.c
////////////////////Hardware interrupt handlers and exception handlers should have their DPL set to 0, The system call handler should have its DPL set to 3 s

#include "lib.h"


#define SYSTEM_CALL 0x80
#define KERNAL_SEG 0x0010
void exception_0_de(){
	clear();
	printf("Divide Error Exception");
}

void exception_1_db(){
	clear();
	printf("Debug Exception");
}

void exception_2_nmi(){
	clear();
	printf("Non Maskable Interrupt");
}

void exception_3_bp(){
	clear();
	printf("Breakpoint Exception");
}

void exception_4_of(){
	clear();
	printf("Overflow Exception");
}

void exception_5_br(){
	clear();
	printf("BOUND Range Exceeded Exception");
}

void exception_6_ud(){
	clear();
	printf("—Invalid Opcode Exception");
}

void exception_7_nm(){
	clear();
	printf("Device Not Available Exception");
}

void exception_8_df(){
	clear();
	printf("Double Fault Exception");
}

void exception_9_cso(){
	clear();
	printf("Coprocessor Segment Overrun");
}

void exception_10_ts(){
	clear();
	printf("Invalid TSS Exception");
}

void exception_11_np(){
	clear();
	printf("Segment Not Present");
}

void exception_12_ss(){
	clear();
	printf("Stack Fault Exception");
}

void exception_13_gp(){
	clear();
	printf("General Protection Exception");
}

void exception_14_pf(){
	clear();
	printf("Page-Fault Exception ");
}
void exception_15_rs(){
	clear();
	printf("Reserved");
}

void exception_16_mf(){
	clear();
	printf("x87 FPU Floating-Point Error");
}

void exception_17_ac(){
	clear();
	printf("Alignment Check Exception");
}

void exception_18_mc(){
	clear();
	printf("Machine-Check Exception");
}

void exception_19_xf(){
	clear();
	printf("SIMD Floating-Point Exception");
}

void exception_undefined(){
	clear();
	printf("Undefined Exception!");
}

void idt_init(){
	int i;
	for (i = 0; i< NUM_VEC; i++){
		if (i != SYSTEM_CALL) idt[i].dpl=0;
		else if(i>) idt[i].dpl = 3;
		idt[i].seg_selector = KERNAL_SEG;
		
	}
}
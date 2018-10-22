////////////////////TODO: empty exception handler, initialization, CALL x86desc SET_IDT_ENTRY, assmebly, call init in kernal.c
////////////////////Hardware interrupt handlers and exception handlers should have their DPL set to 0, The system call handler should have its DPL set to 3 s
//////////////set keyboard rtc handler also
#include "lib.h"
#include "idt.h"
#include "x86_desc.h"
//#include "interrupt_service_wrapper.h"
#define SYSTEM_CALL 0x80
#define KERNAL_SEG 0x0010
void exception_0_de(){
	cli();
	clear();
	
	printf("Divide Error Exception");
	while(1);
	sti();
}

void exception_1_db(){
	cli();
	clear();
	
	printf("Debug Exception");
	while(1);
	sti();
}

void exception_2_nmi(){
	cli();
	clear();
	
	printf("Non Maskable Interrupt");
	while(1);
	sti();
}

void exception_3_bp(){
	cli();
	clear();
	
	printf("Breakpoint Exception");
	while(1);
	sti();
}

void exception_4_of(){
	cli();
	clear();
	
	printf("Overflow Exception");
	while(1);
	sti();
}

void exception_5_br(){
	cli();
	clear();
	
	printf("BOUND Range Exceeded Exception");
	while(1);
	sti();
}

void exception_6_ud(){
	cli();
	clear();
	
	printf("—Invalid Opcode Exception");
	while(1);
	sti();
}

void exception_7_nm(){
	cli();
	clear();
	
	printf("Device Not Available Exception");
	while(1);
	sti();
}

void exception_8_df(){
	cli();
	clear();
	
	printf("Double Fault Exception");
	while(1);
	sti();
}

void exception_9_cso(){
	cli();
	clear();
	
	printf("Coprocessor Segment Overrun");
	while(1);
	sti();
}

void exception_10_ts(){
	cli();
	clear();
	
	printf("Invalid TSS Exception");
	while(1);
	sti();
}

void exception_11_np(){
	cli();
	clear();
	
	printf("Segment Not Present");
	while(1);
	sti();
}

void exception_12_ss(){
	cli();
	clear();
	
	printf("Stack Fault Exception");
	while(1);
	sti();
}

void exception_13_gp(){
	cli();
	clear();
	
	printf("General Protection Exception");
	while(1);
	sti();
}

void exception_14_pf(){
	cli();
	clear();
	
	printf("Page-Fault Exception ");
	while(1);
	sti();
}
void exception_15_rs(){
	cli();
	clear();
	
	printf("Reserved");
	while(1);
	sti();
}

void exception_16_mf(){
	cli();
	clear();
	
	printf("x87 FPU Floating-Point Error");
	while(1);
	sti();
}

void exception_17_ac(){
	cli();
	clear();

	printf("Alignment Check Exception");
	while(1);
	sti();
}

void exception_18_mc(){
	cli();
	clear();

	printf("Machine-Check Exception");
	while(1);
	sti();
}

void exception_19_xf(){
	cli();
	clear();
	
	printf("SIMD Floating-Point Exception");
	while(1);
	sti();
}


/*user defined handlers or undefined*/
void exception_user_defined(){
	cli();
	clear();
	printf("User Defined or Undefined Exception!");
	while(1);
	sti();
}



///////////not sure whether I can write the assembly this way due to https://wiki.osdev.org/Interrupt_Service_Routines

void idt_init(){
	int i;
	for (i = 0; i< NUM_VEC; i++){
		
		idt[i].seg_selector = KERNAL_SEG;

		idt[i].reserved4 =0;
		if (i<32) idt[i].reserved3 =1; //for intel defined  exception handlers, set to trap gate
		else idt[i].reserved3 = 0; // otherwise, set to interrupt gates
		idt[i].reserved2 =1;
		idt[i].reserved1 =1;
		idt[i].size = 1;
		idt[i].reserved0 =0;
		if (i != SYSTEM_CALL) idt[i].dpl=0;	
		else idt[i].dpl = 3; //for system call handler, DPL is set to 3
		idt[i].present =1;
	}
	
	SET_IDT_ENTRY(idt[0], exception_0_de);
	SET_IDT_ENTRY(idt[1], exception_1_db);
	
	idt[2].reserved3 = 0;
	SET_IDT_ENTRY(idt[2], exception_2_nmi);	
	
	idt[3].dpl = 3;						
	SET_IDT_ENTRY(idt[3], exception_3_bp);
	
	idt[4].dpl =3;						//overflow has dpl =3
	SET_IDT_ENTRY(idt[4], exception_4_of);
	
	idt[5].dpl = 3;						//interrupt gate
	SET_IDT_ENTRY(idt[5], exception_5_br);

	
	SET_IDT_ENTRY(idt[6], exception_6_ud);
	SET_IDT_ENTRY(idt[7], exception_7_nm);
	SET_IDT_ENTRY(idt[8], exception_8_df);
	SET_IDT_ENTRY(idt[9], exception_9_cso);
	SET_IDT_ENTRY(idt[10], exception_10_ts);
	SET_IDT_ENTRY(idt[11], exception_11_np);
	SET_IDT_ENTRY(idt[12], exception_12_ss);
	SET_IDT_ENTRY(idt[13], exception_13_gp);
	
	idt[14].reserved3 = 0;				//interrupt gate
	SET_IDT_ENTRY(idt[14], exception_14_pf);
	SET_IDT_ENTRY(idt[15], exception_15_rs);
	SET_IDT_ENTRY(idt[16], exception_16_mf);
	SET_IDT_ENTRY(idt[17], exception_17_ac);
	SET_IDT_ENTRY(idt[18], exception_18_mc);
	SET_IDT_ENTRY(idt[19], exception_19_xf);
	SET_IDT_ENTRY(idt[0x21], keyboard_handler);
	SET_IDT_ENTRY(idt[0x28], rtc_int_handler);
	
	idt[SYSTEM_CALL].reserved3 = 1;		//system call handler is trap gate
	for (i = 20; i < NUM_VEC; i++){
		SET_IDT_ENTRY(idt[i], exception_user_defined);
	}
}

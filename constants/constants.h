#define MODNAME "syscall_adder"
#define MODNAME_R "syscall_remover"
#define NUM_ENTRIES 1024
//This file will contain the macros that will be used to call
//a custom syscall as if it was an 'embedded' one.
//Eg. sys_call_adder("syscall_name") will be called inside the kernel module
//that contains the new syscall to add. (TODO automate the process)
#define MACRO_DIR "/home/ezio"
#define CUSTOM_SYSCALL_MACROS_RAW "%s/custom_syscall_macros.h"

#define ADDRESS_MASK 0xfffffffffffff000//to migrate

#define START 			0xffffffff00000000ULL		// use this as starting address --> this is a biased search since does not start from 0xffff000000000000
#define MAX_ADDR		0xfffffffffff00000ULL
#define FIRST_NI_SYSCALL	134
#define SECOND_NI_SYSCALL	174
#define THIRD_NI_SYSCALL	182 
#define FOURTH_NI_SYSCALL	183
#define FIFTH_NI_SYSCALL	214	
#define SIXTH_NI_SYSCALL	215	
#define SEVENTH_NI_SYSCALL	236	

#define ENTRIES_TO_EXPLORE 256

MODULE_AUTHOR("Ezio Emanuele Ditella");
MODULE_DESCRIPTION("This module will create a syscall that will be used \
					to add new syscalls.");


DEFINE_MUTEX(mod_mutex);

char CUSTOM_SYSCALL_MACROS[512];
char *syscall_names[NUM_ENTRIES] = { [ 0 ... NUM_ENTRIES-1 ] = 0 };
int syscall_cts_numbers[NUM_ENTRIES] = { [ 0 ... NUM_ENTRIES-1 ] = 0 };
int total_syscall_added = 0;
int add_indx, rem_indx;
int uninstalling = 0;


unsigned long *hacked_ni_syscall=NULL;
unsigned long **hacked_syscall_tbl=NULL;

unsigned long sys_call_table_address = 0x0;
module_param(sys_call_table_address, ulong, 0660);

unsigned long sys_ni_syscall_address = 0x0;
module_param(sys_ni_syscall_address, ulong, 0660);


int line_len(char *macro_line)
{
	int len = 1;
	while(*(macro_line+len) != '\n'){
		len++;
		if(len > 1025){
			return -1;
		}
	}

	return len+1;
}

int insert_macro_line(int syscall_num, char *macro_line)
{
	int write_ret;
	struct file *f;
	char *line1raw = "//%d\n";
	char *line3 = "//end\n";
	char line1[7] = { [ 0 ... 6 ] = 0 };

	//opening  macro file, creating if needed. (root permission, system readable)
	f = filp_open(CUSTOM_SYSCALL_MACROS, O_CREAT|O_APPEND|O_RDWR, 0666);
	if(IS_ERR(f)){
		printk(KERN_ERR "%s: Cannot open/create macro file\n", MODNAME);
		return -1;
	}

	if(snprintf(line1, 7, line1raw, syscall_num) <= 0){
		printk(KERN_ERR "%s: snprintf line1\n", MODNAME);
		filp_close(f, NULL);
		return -1;
	}

	//appending 3 macro line, as described in docs
	write_ret = kernel_write(f, line1, line_len(line1), &f->f_pos);
	if(write_ret <= 0){
		printk(KERN_ERR "%s: Cannot write first line of macro: %s, %d\n", MODNAME, line1, line_len(line1));
		filp_close(f, NULL);
		return -1;
	}
	write_ret = kernel_write(f, macro_line, line_len(macro_line), &f->f_pos);
	if(write_ret <= 0){
		printk(KERN_ERR "%s: Cannot write second line of macro\n", MODNAME);
		filp_close(f, NULL);
		return -1;
	}
	write_ret = kernel_write(f, line3, line_len(line3), &f->f_pos);
	if(write_ret <= 0){
		printk(KERN_ERR "%s: Cannot write third line of macro\n", MODNAME);
		filp_close(f, NULL);
		return -1;
	}

	filp_close(f, NULL);
	return 0;
}


int find_syscalltable_free_entry(void)
{
	int i;

	for(i = 0; i < ENTRIES_TO_EXPLORE; i += 1){

		if(hacked_syscall_tbl[i] == (unsigned long *)hacked_ni_syscall){
			break;
		}
	}
	if(i == NUM_ENTRIES){
		return -1;
	}
	return i;
}

int update_syscalltable_entry(void* custom_syscall, char* syscall_name)
{
	int syst_entry;
	int SYSCALL_NAME_MAX_LEN = 1024;

	syst_entry = find_syscalltable_free_entry();
	if(syst_entry == -1){
		printk(KERN_DEBUG "%s: Free entry not found", MODNAME);
		return -1;
	}
	printk(KERN_DEBUG "%s: Index found: %d\n",MODNAME, syst_entry);

	cr0 = read_cr0();
	unprotect_memory();
	hacked_syscall_tbl[syst_entry] = (unsigned long *)custom_syscall;
	protect_memory();

	printk(KERN_DEBUG "%s: Syscall added !",MODNAME);

	//update local arrays
	if((syscall_names[total_syscall_added] = kmalloc(SYSCALL_NAME_MAX_LEN, GFP_KERNEL)) == NULL){
		printk(KERN_ERR "%s: Unable to kmalloc syscall_names", MODNAME);
		return -1;
	}
	if(memcpy(syscall_names[total_syscall_added], syscall_name, SYSCALL_NAME_MAX_LEN) == NULL){
		printk(KERN_ERR "%s: Unable to memcpy", MODNAME);
		return -1;
	}
	syscall_cts_numbers[total_syscall_added] = syst_entry;
	total_syscall_added++;

	return syst_entry;
}

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/binfmts.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/seq_file.h>

#include "pnb.h"
#include "ports.c"
#include "process.c"

static struct task_struct * proc_thrd;
extern struct struct_res file_res;

static int file_res_show(struct seq_file *m, void *v)
{
	seq_printf(m, "PID\tIN_BDTH\tOUT_BDTH\n");
	return 0;
}

static int file_res_open(struct inode *inode, struct file *file)
{
	return single_open(file, file_res_show, NULL);
}

static const struct file_operations file_res_fops =
{
	.owner = THIS_MODULE,
	.open = file_res_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static void reset_var(void)
{
	memset(loc_ports, 0, sizeof(loc_ports));
	memset(rem_ports, 0, sizeof(rem_ports));

	proc_list = kmalloc(sizeof(struct Process), GFP_KERNEL);
}

static void write_results(void)
{
	int i;
	for (i = 0; i < sizeof(proc_list) / sizeof(proc_list[0]); i++){

		if (proc_list[i]->loc_ports[0] < 1){
			continue;
		}

		//printk("Lol : %d\n", proc_list[i].loc_ports[0]);
	}

	//each proc -> += nb packets in/out
	// fd_res
}

static void init_files(void)
{
	int i;

	if (!(files_inodes = kmalloc(sizeof(struct file) * size_to_read, GFP_KERNEL)))
		goto err;

	for (i = 0; i < size_to_read; i++)
	{
		files_inodes[i] = filp_open(to_read[i], O_RDONLY, 0);

		if(IS_ERR(files_inodes[i]))
			goto err;
	}

	if (!proc_create(file_res.name, S_IRWXU, NULL, &file_res_fops))
		goto err;

	file_res.fd = filp_open(file_res.path, O_RDWR | O_APPEND, S_IRWXU);

	if(IS_ERR(file_res.fd))
		goto err;

	return;
err:
	printk(KERN_ALERT "init_files error !\n");
}

static void clean_files(void)
{
	int i_toread;

	for (i_toread = 0; i_toread < size_to_read; i_toread++)
		filp_close(files_inodes[i_toread], 0);

	filp_close(file_res.fd, 0);
	remove_proc_entry(file_res.name, NULL);
}

int timer_function(void *data)
{
	//while (!kthread_should_stop())
	//{
		//TODO is_new_data
		refresh_process();
		write_results();
		reset_var();

		msleep(1000);
	//}

	return 0;
}

static int __init pnb_init(void)
{
	//uint8
	//ssize
	//mem leak

	reset_var();

	init_files();
	init_hook();
	//proc_thrd = kthread_run(&timer_function, NULL, "process-refresher");

	timer_function(NULL);

	return 0;
}

static void __exit pnb_cleanup(void)
{
	clean_hook();
	clean_files();
	//kthread_stop(proc_thrd);

	kfree(proc_list);
	kfree(files_inodes);
	kfree(proc_thrd);
}

module_init(pnb_init);
module_exit(pnb_cleanup);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Network bandwidth stats per process");

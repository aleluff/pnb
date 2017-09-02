#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/binfmts.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/hrtimer.h>
#include <linux/sched.h>

#include "pnb.h"
#include "ports.c"
#include "process.c"

static struct hrtimer htimer;
static ktime_t kt_periode;
static struct file *fd_res;

static void reset_var(void)
{
	memset(loc_ports, 0, sizeof(loc_ports));
	memset(rem_ports, 0, sizeof(rem_ports));

	proc_list = kmalloc(sizeof(struct Process), GFP_KERNEL);
}

static void write_results(void)
{
	int i;
	for (i = 0; i < MAX_PORTS; i++){
		if (loc_ports[i] > 1){
			//printk("loc %d : %d\n", i, loc_ports[i]);
		}
		if (rem_ports[i] > 1){
			//printk("rem %d : %d\n", i, rem_ports[i]);
		}
	}

//each proc -> += nb packets in/out
// fd_res
}

static void init_files(void)
{
	int i;

	files_inodes = kmalloc(sizeof(struct file) * size_to_read, GFP_KERNEL);

	for (i = 0; i < size_to_read; i++)
	{
		files_inodes[i] = filp_open(to_read[i], O_RDONLY, 0);

		if(IS_ERR(files_inodes[i]))
			goto err;
	}

	//TODO
	fd_res = filp_open(file_res, O_CREAT | O_RDWR | O_APPEND, S_IRWXU);

	if(IS_ERR(fd_res))
			goto err;

err:
	printk(KERN_ALERT "filp_open error !\n");
}

static void clean_files(void)
{
	int i;

	for (i = 0; i < size_to_read; i++)
		filp_close(files_inodes[i], 0);

	//unlink fd_res
	filp_close(fd_res, 0);
}

static enum hrtimer_restart timer_function(struct hrtimer * timer)
{
	refresh_process();
	write_results();
	reset_var();

	hrtimer_forward_now(timer, kt_periode);

	return HRTIMER_RESTART;
}

static void timer_init(void)
{
	kt_periode = ktime_set(1, 0);
	hrtimer_init (&htimer, CLOCK_REALTIME, HRTIMER_MODE_REL);
	htimer.function = timer_function;
	hrtimer_start(&htimer, kt_periode, HRTIMER_MODE_REL);
}

static void timer_cleanup(void)
{
	hrtimer_cancel(& htimer);
}

static int __init pnb_init(void)
{
	reset_var();

	init_files();
	init_hook();
	timer_init();

	//kernel min version

	return 0;
}

static void __exit pnb_cleanup(void)
{
	timer_cleanup();
	clean_hook();
	clean_files();

	kfree(proc_list);
}

module_init(pnb_init);
module_exit(pnb_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Biche");
MODULE_DESCRIPTION("Network bandwidth stats per process");

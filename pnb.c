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

char ** read_file(char * filename)
{
	struct file *fp;
	char *tok;
	char *buf;
	char **list;
	int i;

	printk(KERN_ALERT "%s\n", fp->f_op->read);
	//fp = filp_open(filename, O_RDONLY, 0);

	if(IS_ERR(fp))
		printk(KERN_ALERT "filp_open %s error!!.\n", filename);
	else
	{
		if(!(buf = kmalloc(BINPRM_BUF_SIZE, GFP_KERNEL))) {
			printk(KERN_ALERT "KMALLOC");
			return list;
		}

		//kernel_read(fp, 0, buf, BINPRM_BUF_SIZE);
		//list = kmalloc(sizeof(buf), GFP_KERNEL);

		//while ((tok = strsep(&buf, "\n"))) {
		//	list[i++] = tok;
		//}

		//filp_close(fp, NULL);
	}

	kfree(buf);
	kfree(tok);
	kfree(fp);

	return list;
}

static void reset_var(void)
{
	memset(loc_ports, 0, sizeof(loc_ports));
	memset(rem_ports, 0, sizeof(rem_ports));
	memset(proc_list, 0, sizeof(proc_list));
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

}

static bool is_new_data(void)
{
	int i;
	for (i = 0; i < MAX_PORTS; i++){
		if (loc_ports[i] > 1){
			return true;
		}
		if (rem_ports[i] > 1){
			return true;
		}
	}

	return false;
}

static enum hrtimer_restart timer_function(struct hrtimer * timer)
{
	//if (is_new_data())
	{
		refresh_process();
		write_results();
		reset_var();
	}

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
	init_hook();
	timer_init();

	//kernel min version
	//clear static fct
	//test limit array ex : +20port par proc

	return 0;
}

static void __exit pnb_cleanup(void)
{
	timer_cleanup();
	clean_hook();
}

module_init(pnb_init);
module_exit(pnb_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Biche");
MODULE_DESCRIPTION("Network bandwidth stats per process");

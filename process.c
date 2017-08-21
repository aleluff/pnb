#include <linux/sched/signal.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/rcupdate.h>
#include <linux/fdtable.h>
#include <linux/dcache.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

extern struct Process proc_list[MAX_PROC];

static char * read_file(char * filename)
{
	struct file *f;
	int buf_lgth = 3000;
	char *buf = vmalloc(sizeof(char) * buf_lgth);
	mm_segment_t fs;
	int i;

	for(i = 0; i < buf_lgth; i++)
		buf[i] = 0;

	f = filp_open(filename, O_RDONLY, 0);

	if(f == NULL)
		printk(KERN_ALERT "filp_open error!!.\n");
	else
	{
		fs = get_fs();
		set_fs(get_ds());

		f->f_op->read(f, buf, buf_lgth, &f->f_pos);

		set_fs(fs);
	}
	filp_close(f,NULL);
	return buf;
}

static unsigned long * get_inodes_ports(void)
{
	char to_read[][30] = {
		"/proc/net/tcp",
		"/proc/net/tcp6",
		"/proc/net/udp",
		"/proc/net/udp6"
	};
	int i;
	int arr_size = sizeof(to_read);
	unsigned long *inodes = kmalloc(sizeof(unsigned long) * MAX_PROC * arr_size, GFP_KERNEL);

	if (!inodes)
		return NULL;

	for (i = 0; i < arr_size / sizeof(to_read[0]); i++){

		char * res = read_file(to_read[i]);

		//fscanf(fp, "%d %s %c %d %d %d %d %d %u %lu ...", &pid, &name, &ppid, &dummy, &dummy, &dummy, ...);
	}

	return inodes;
}

//TODO check /task/fd/
static unsigned long * get_inodes_of_process(struct task_struct *proc)
{
	unsigned long *inodes = kmalloc(sizeof(unsigned long) * SMALL_ARRAY_SIZE * 2, GFP_KERNEL);
	int i = 0;

	if (!inodes)
		return NULL;

	for (i = 0; i < proc->files->fdt->max_fds; i++)
		inodes[i] = proc->files->fdt->fd[i]->f_path.dentry->d_inode->i_ino;

	return inodes;
}

static void refresh_process(void)
{
	//unsigned long * inodes_ports = get_inodes_ports();
	struct task_struct *proc;

	for_each_process(proc)
	{
		struct Process tmp;

		unsigned long * proc_inodes = get_inodes_of_process(proc);

		proc_list[proc->pid] = tmp;
	}
}

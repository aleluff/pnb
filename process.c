#include <linux/sched/signal.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/rcupdate.h>
#include <linux/fdtable.h>
#include <linux/dcache.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

extern struct Process proc_list[MAX_PROC];

static char ** read_file(char * filename)
{
	struct file *f;
	int buf_lgth = 3000;
	char *tok;
	char *buf = kmalloc(sizeof(char) * buf_lgth, GFP_KERNEL);
	char **list = kmalloc(sizeof(buf), GFP_KERNEL);
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
		i = 0;

		while ((tok = strsep(&buf, "\n"))) {
			list[i++] = tok;
		}
	}
	filp_close(f,NULL);
	return list;
}

static struct Inodes_ports * get_inodes_ports(void)
{
	struct Inodes_ports *list = kmalloc(sizeof(struct Inodes_ports) * MAX_PORTS, GFP_KERNEL);
	struct Inodes_ports tmp;
	char to_read[][30] = {
		"/proc/net/tcp",
		"/proc/net/tcp6",
		"/proc/net/udp",
		"/proc/net/udp6"
	};
	char * bs;
	char ** res;
	int i;
	int y;
	int z = 0;

	if (!list)
		return NULL;

	for (i = 0; i < sizeof(to_read) / sizeof(to_read[0]); i++){

		y = 1;
		res = read_file(to_read[i]);

		while(res[y] != NULL){

			sscanf(res[y], "%s: %s:%u %s:%u %s %s %s %s %s %s %s %u %s %s %s %s",
		      		&bs, &bs, &tmp.loc_port, &bs, &tmp.rem_port, &bs, &bs, &bs, &bs, &bs, &bs, &tmp.ino, &bs, &bs, &bs, &bs);

			list[z] = tmp;
			y++;
			z++;
		}
	}

	return list;
}

//TODO check /task/fd/
static struct inode * get_inodes_of_process(struct task_struct *proc)
{
	struct inode *list = kmalloc(sizeof(struct inode) * SMALL_ARRAY_SIZE * 2, GFP_KERNEL);
	struct inode test;
	int i = 0;

	if (!list)
		return NULL;

	for (i = 0; i < proc->files->fdt->max_fds; i++){

		//list[i] = test;//proc->files->fdt->fd[i]->f_path.dentry->d_inode;
	}

	return list;
}

static void refresh_process(void)
{
	struct Inodes_ports * inodes_ports = get_inodes_ports();
	struct task_struct *proc;

	for_each_process(proc)
	{
		struct Process tmp;

		//struct inode * proc_inodes = get_inodes_of_process(proc);

		proc_list[proc->pid] = tmp;
	}
}

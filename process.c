#include <linux/sched/signal.h>
#include <linux/fdtable.h>

extern struct Process proc_list[MAX_PROC];

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
	unsigned int tmpino;
	char * bs = kmalloc(sizeof(char) * 30, GFP_KERNEL);
	char ** res;
	int i;
	int y;
	int z = 0;

	if (!list)
		return NULL;

	for (i = 0; i < sizeof(to_read) / sizeof(to_read[0]); i++){

		y = 1;
		res = read_file(to_read[i]);

		/*while(res[y] != NULL){

			sscanf(res[y], "%s: %s:%u %s:%u %s %s %s %s %s %s %u %s %s %s",
		      		bs, bs, &tmp.loc_port, bs, &tmp.rem_port, bs, bs, bs, bs, bs, bs, &tmpino, bs, bs, bs);

			tmp.ino = (long unsigned int)tmpino;

			list[z] = tmp;
			y++;
			z++;
		}*/
	}

	kfree(res);
	kfree(bs);

	return list;
}

//TODO check /task/fd/
static struct inode * get_inodes_of_process(struct task_struct *proc)
{
	struct inode *list = kmalloc(sizeof(struct inode) * 500, GFP_KERNEL);
	struct fdtable *fdt;
	struct file *file;
	int i = 0;

	if (!list)
		return NULL;

	rcu_read_lock();
	fdt = files_fdtable(proc->files);

	for (i = 0; i < fdt->max_fds; i++)
	{
		file = fcheck_files(proc->files, i);
		if (!file) {
			continue;
		}

		list[i].i_ino = fdt->fd[i]->f_path.dentry->d_inode->i_ino;
	}

	rcu_read_unlock();
	kfree(fdt);
	kfree(file);

	return list;
}

static void refresh_process(void)
{
	struct Inodes_ports * inodes_ports;
	struct inode * proc_inodes;
	struct Process tmp;
	struct task_struct *proc;

	inodes_ports = get_inodes_ports();

	for_each_process(proc)
	{
		//proc_inodes = get_inodes_of_process(proc);

		proc_list[proc->pid] = tmp;
	}

	kfree(proc_inodes);
	kfree(inodes_ports);
}

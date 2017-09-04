//TODO if version > 4.11
#include <linux/sched/signal.h>
#include <linux/fdtable.h>

extern struct Process ** proc_list;

struct Inodes_ports
{
	unsigned long		ino;
	unsigned int		loc_port;
	unsigned int		rem_port;
};

char ** read_file(struct file * fp)
{
	char ** list;
	char * buf;
	char * eol;
	int buf_lgth = 10000;/*BINPRM_BUF_SIZE*/
	int i_list = 0;

	if(!(buf = kmalloc(buf_lgth, GFP_KERNEL)))
		return NULL;

	kernel_read(fp, 0, buf, buf_lgth);

	if(!(list = kmalloc(sizeof(buf), GFP_KERNEL)))
		return NULL;

	while(1)
	{
		eol = strchr(buf, '\n');
		if (!eol)
			break;
printk("%d\n", strlen(eol));
		list[i_list++] = eol;
		buf += strlen(eol);
	};

	return list;
}

static struct Inodes_ports * get_inodes_ports(void)
{
	struct Inodes_ports * list;
	char * bs;
	char ** res;
	int i_list = 0;
	int i_toread;
	int i_res;

	if (!(list = kmalloc(sizeof(struct Inodes_ports) * MAX_PORTS, GFP_KERNEL)))
		return NULL;

	if (!(bs = kmalloc(sizeof(char) * 30, GFP_KERNEL)))
		return NULL;

	for (i_toread = 0; i_toread < size_to_read; i_toread++)
	{
		res = read_file(files_inodes[i_toread]);

		printk("-------------------------\n");

		for (i_res = 1; i_res < sizeof(res); i_res++)
		{

			/*sscanf(res[i_res], "%s: %s:%x %s:%x %s %s %s %s %s %s %ld %s\n",
		      		bs, bs, &list[i_list].loc_port, bs, &list[i_list].rem_port,
			       bs, bs, bs, bs, bs, bs, &list[i_list].ino, bs);*/

			//printk("%ld %d %d\n", list[i_list].ino, list[i_list].loc_port, list[i_list].rem_port);
			i_list++;
		}

	}

	kfree(bs);
	kfree(res);

	return list;
}

//TODO check /task/fd/
static unsigned int * get_inodes_of_process(struct task_struct * proc)
{
	unsigned int * list;
	struct fdtable * fdt;
	struct file * file;
	int i_fdt = 0;

	printk("%d\n", proc->pid);
	if (IS_ERR(proc->files->fdt))
		goto err;

	if (!(list = kmalloc(sizeof(unsigned int) * proc->files->count.counter, GFP_KERNEL)))
		goto err;

	rcu_read_lock();

	fdt = files_fdtable(proc->files);

	for (i_fdt = 0; i_fdt < fdt->max_fds; i_fdt++)
	{
		file = fcheck_files(proc->files, i_fdt);
		if (IS_ERR(file))// || !atomic_long_inc_not_zero(&file->f_count)
			continue;

		list[i_fdt] = file->f_path.dentry->d_inode->i_ino;

		//kfree(file);
	}

	rcu_read_unlock();

err:
	kfree(fdt);
	if (!IS_ERR(file))
		kfree(file);

	return list;
}

void refresh_process(void)
{
	struct Inodes_ports * inodes_ports;
	//struct Process * tmp;
	struct task_struct * proc;
	unsigned int * proc_inodes;
	//int i = 0;

	inodes_ports = get_inodes_ports();
	if (IS_ERR(inodes_ports))
		return;

	for_each_process(proc)
	{
		//proc_inodes = get_inodes_of_process(proc);
		if (IS_ERR(proc_inodes))
			continue;

		//for (i = 0; i < sizeof(proc_inodes) / sizeof(proc_inodes[0]); i++){
		//	printk("%d\n", proc_inodes[i]);
		//}

		//Ne pas ajouter proc si pas de ports
		//gerer tmp->next

		//proc_list[proc->pid] = tmp;

		kfree(proc_inodes);
	}

	kfree(inodes_ports);
}

#include <linux/fdtable.h>

extern struct Process ** proc_list;

struct Inodes_ports {
	unsigned long		ino;
	unsigned int		loc_port;
	unsigned int		rem_port;
};

char * read_file(struct file * fp)
{
	char * buf;

	if(!(buf = kmalloc(BINPRM_BUF_SIZE, GFP_KERNEL))) {
		return NULL;
	}

	kernel_read(fp, 0, buf, BINPRM_BUF_SIZE);

	return buf;
}

static struct Inodes_ports ** get_inodes_ports(void)
{
	struct Inodes_ports ** list = kmalloc(sizeof(struct Inodes_ports) * MAX_PORTS, GFP_KERNEL);
	char * bs = kmalloc(sizeof(char) * 30, GFP_KERNEL);
	char * res;
	int i;
	int offset;
	int readed_char;
	int z = 0;

	if (!list)
		return NULL;

	for (i = 0; i < size_to_read; i++){

		res = read_file(files_inodes[i]);

		sscanf(res, "%s: %s:%u %s:%u %s %s %s %s %s %s %u %s %s %s\n",
		      		bs, bs, &list[z]->loc_port, bs, &list[z]->rem_port,
			       bs, bs, bs, bs, bs, bs, &list[z]->ino, bs, bs, bs);

			z++;
/*
		int offset = 0, readCharCount;
		    while(sscanf(str + offset, "%c%d%*c%n", &col, &line, &readed_char) == 2){
			printf("%c, %d\n", col, line);
			offset += readCharCount;
		    }*/

	}

	kfree(bs);
	kfree(res);

	return list;
}

//TODO check /task/fd/
static unsigned int * get_inodes_of_process(struct task_struct *proc)
{
	unsigned int * list = kmalloc(sizeof(struct inode) * 500, GFP_KERNEL);
	struct fdtable *fdt;
	struct file *file;
	int i = 0;

	if (!list)
		return NULL;

	fdt = files_fdtable(proc->files);

	for (i = 0; i < fdt->max_fds; i++)
	{
		file = fcheck_files(proc->files, i);
		if (!file)
			continue;

		list[i] = fdt->fd[i]->f_path.dentry->d_inode->i_ino;

		kfree(file);
	}

	kfree(fdt);

	return list;
}

static void refresh_process(void)
{
	struct Inodes_ports ** inodes_ports;
	struct Process tmp;
	struct task_struct *proc;
	unsigned int * proc_inodes;
	int i;

	//inodes_ports = get_inodes_ports();

	for_each_process(proc)
	{
		proc_inodes = get_inodes_of_process(proc);

		//Ne pas ajouter proc si pas de ports
		//gerer tmp->next

		//proc_list[proc->pid] = tmp;

		kfree(proc_inodes);
	}

	kfree(inodes_ports);
}

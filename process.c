#include <linux/sched/signal.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/rcupdate.h>
#include <linux/fdtable.h>
#include <linux/dcache.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

struct Process {
	int loc_ports[SMALL_ARRAY_SIZE];
	int rem_ports[SMALL_ARRAY_SIZE];
};

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
	{
		printk(KERN_ALERT "filp_open error!!.\n");
	}
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

static int * get_inodes_ports(void)
{
	char to_read[][30] = {
		"/proc/net/tcp",
		"/proc/net/tcp6",
		"/proc/net/udp",
		"/proc/net/udp6"
	};
	int i;
	int arr_size = sizeof(to_read);
	int *inodes = kmalloc(GFP_KERNEL, sizeof(int) * MAX_PROC * arr_size);

	for (i = 0; i < arr_size / sizeof(to_read[0]); i++){

		char * res = read_file(to_read[i]);

		//parse
	}

	return inodes;
}

//TODO check /task/fd/
static int * get_inodes_of_process(struct files_struct *current_files)
{
	struct fdtable *files_table;
	struct path files_path;
	int *inodes = kmalloc(GFP_KERNEL, sizeof(int) * SMALL_ARRAY_SIZE * 2);
	char *buf = (char *)kmalloc(GFP_KERNEL, 100 * sizeof(char));
	int i = 0;
	char *cwd;

	files_table = files_fdtable(current_files);

	while(files_table->fd[i] != NULL)
	{
		//filtre network

		//A revoir
		files_path = files_table->fd[i]->f_path;
		cwd = d_path(&files_path, buf, 100 * sizeof(char));

		printk(KERN_DEBUG "Open file with fd %d  %s", i, cwd);

		i++;
	}
	return inodes;
}

static void refresh_process(void)
{
	//int * inodes_ports = get_inodes_ports();
	struct task_struct *proc;

	for_each_process(proc)
	{
		struct Process tmp;

		int * proc_inodes = get_inodes_of_process(proc->files);

		proc_list[proc->pid] = tmp;
	}
}

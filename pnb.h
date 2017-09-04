#define MAX_PORTS 65535

#include <linux/proc_fs.h>

struct Process {
	pid_t			pid;
	unsigned int		loc_ports[500];
	unsigned int		rem_ports[500];
	struct Process *	next;
};

struct struct_res {
	const char *		name;
	const char *		path;
	struct file *		fd;
};
struct struct_res file_res 	=
{
	.name = "net_bdth",
	.path = "/proc/net_bdth"

};
const char to_read[][30]	=
{
	"/proc/net/tcp",
	"/proc/net/tcp6",
	"/proc/net/udp",
	"/proc/net/udp6"
};
const int size_to_read		= sizeof(to_read) / sizeof(to_read[0]);

//gerer ->next
unsigned int loc_ports[MAX_PORTS];
unsigned int rem_ports[MAX_PORTS];

struct Process ** proc_list;
struct file ** files_inodes;

extern char ** read_file(struct file * fp);

#define MAX_PORTS 65535

struct Process {
	pid_t			pid;
	unsigned int		loc_ports[500];
	unsigned int		rem_ports[500];
	struct Process *	next;
};

const char * file_res		= "/var/log/my_kern_module.log";//"/proc/net/bandwidth";
const char to_read[][30]	=
{
	"/proc/net/tcp",
	"/proc/net/tcp6",
	"/proc/net/udp",
	"/proc/net/udp6"
};
const int size_to_read		= sizeof(to_read) / sizeof(to_read[0]);

unsigned int loc_ports[MAX_PORTS];
unsigned int rem_ports[MAX_PORTS];

struct Process ** proc_list;
struct file ** files_inodes;

extern char * read_file(struct file * fp);

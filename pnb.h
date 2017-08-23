#define MAX_PORTS  65535
#define MAX_PROC  32768

struct Process {
	unsigned int loc_ports[1];
	unsigned int rem_ports[1];
};

struct Inodes_ports {
	long unsigned int ino;
	unsigned int loc_port;
	unsigned int rem_port;

};

unsigned int loc_ports[MAX_PORTS];
unsigned int rem_ports[MAX_PORTS];
struct Process proc_list[MAX_PROC];

#define MAX_PORTS  65535
#define MAX_PROC  32768
#define SMALL_ARRAY_SIZE  250

struct Process {
	int loc_ports[SMALL_ARRAY_SIZE];
	int rem_ports[SMALL_ARRAY_SIZE];
};

struct Inodes_ports {
	long unsigned int ino;
	int loc_port;
	int rem_port;

};

int loc_ports[MAX_PORTS];
int rem_ports[MAX_PORTS];
struct Process proc_list[MAX_PROC];

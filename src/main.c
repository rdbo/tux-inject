#include "main.h"
#include "process.h"
#include "arch.h"

int main()
{
	process_t process;
	open_process(getpid(), &process);

	printf("Pid:\t\t%d\n", process.id);
	printf("Parent:\t\t%d\n", process.parent);
	printf("Tracer:\t\t%d\n", process.tracer);
	printf("State:\t\t%c\n", process.state);
	printf("Path:\t\t%s\n", process.exe_path);
	printf("CmdLine:\t%s\n", process.cmdline);
	printf("Owner UID:\t%d\n", process.owner.pw_uid);
	printf("Owner GID:\t%d\n", process.owner.pw_gid);
	printf("Owner Name:\t%s\n", process.owner.pw_name);

	close_process(&process);
	return 0;
}
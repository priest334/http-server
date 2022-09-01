#include "daemonize.h"

#ifdef linux
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif


namespace simple {
	void Daemonize() {
#ifdef linux
		int pid = fork();
		if (pid != 0)
			exit(0);
		setsid();
		int fd = open("/dev/null", O_RDWR, 0);
		dup2(fd, 0);
		dup2(fd, 1);
		if (fd>2)
			close(fd);
#endif
	}
} // namespace hlp



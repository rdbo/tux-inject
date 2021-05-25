#include "libc.h"

static int read_maps(process_t *process, libc_info_t *libc_info)
{
	int ret = 0;
	char *maps_file;
	char *ptr;

	if (!process || !libc_info)
		return ret;

	{
		int fd;
		char buf[255] = { 0 };
		ssize_t chr_count = 0;
		ssize_t maps_len = 0;

		snprintf(buf, ARRLEN(buf) - 1,
			 "/proc/%d/maps", process->id);
		
		fd = open(buf, O_RDONLY);

		if (fd == -1)
			return ret;
		
		while ((chr_count = read(fd, buf,
					 sizeof(buf)) / sizeof(char))) {
			char *old_maps_file = maps_file;

			maps_file = (char *)calloc(
				maps_len + chr_count + 1,
				sizeof(char)
			);

			if (maps_len && old_maps_file) {
				if (maps_file) {
					strncpy(maps_file,
						old_maps_file,
						maps_len);
					
					free(old_maps_file);
				}
			}

			if (!maps_file)
				break;

			strncpy(&maps_file[maps_len], buf, chr_count);

			maps_len += chr_count / sizeof(char);

			maps_file[maps_len] = '\x00';
		}

		if (!maps_file)
			return ret;
	}

	ptr = strstr(maps_file, "/libc.");
	if (!ptr)
		ptr = strstr(maps_file, "/libc-");
	if (!ptr)
		goto L_FREE_RET;
	
	{
		for (; ptr != maps_file; ptr = &ptr[-1])
			if (*ptr == '\n')
				break;
		if (ptr == maps_file)
			goto L_FREE_RET;
	}

	ptr = strchr(ptr, '/');
	{
		char *tmp;
		char *endptr = strchr(ptr, '\n');
		size_t len = 0;
		for (tmp = ptr; tmp != endptr; tmp = &ptr[++len]);

		libc_info->so_path = (char *)calloc(len + 1, sizeof(char));
		if (!libc_info->so_path)
			goto L_FREE_RET;

		strncpy(libc_info->so_path, ptr, len);
		libc_info->so_path[len] = '\x00';
	}

	ret = !ret;
L_FREE_RET:
	free(maps_file);
	return ret;
}

int open_libc(process_t *process, libc_info_t *libc_info)
{
	int ret = 0;

	if (!process || !libc_info)
		return ret;
}

int close_libc(libc_info_t *libc_info)
{

}
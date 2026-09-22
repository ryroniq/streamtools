#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include <utime.h>

#define EVENT_SIZE     ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN  ( 1024 * ( EVENT_SIZE + NAME_MAX + 1 ) )

#define BUF_SIZE       100

void touch(const char *filepath) {
	int fd = open(filepath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		perror("Error opening/creating file");
		exit(EXIT_FAILURE);
	}

	close(fd);

	struct utimbuf new_times;
	new_times.actime = time(NULL);
	new_times.modtime = time(NULL);

	if (utime(filepath, &new_times) < 0) {
		perror("Error updating file times");
		exit(EXIT_FAILURE);
	}
}

void print_content(const char *filepath) {
	FILE *fd;
	char buffer[BUF_SIZE];

	fd = fopen(filepath, "r");
	if (fd == NULL) {
		perror("Unable to open file");
		return;
	}

	size_t bytesRead = fread(buffer, sizeof(char), BUF_SIZE - 1, fd);
	buffer[bytesRead] = '\0';

	printf("\033[2J\033[;1H");
	printf("%s\n", buffer);
	fflush(stdout);

	fclose(fd);
	return;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("%s filepath\n", argv[0]);
		return EXIT_FAILURE;
	}
	const char *filepath = argv[1];

	touch(filepath);
	printf("\033[?25l");
	print_content(filepath);

	int fd;
	int wd;
	char buffer[EVENT_BUF_LEN];

	time_t last_event_time = 0;
	const int debounce_time = 1;

	fd = inotify_init();
	if (fd < 0) {
		perror("inotify_init");
		return EXIT_FAILURE;
	}

	wd = inotify_add_watch(fd, filepath, IN_MODIFY | IN_CLOSE_WRITE);
	if (wd == -1) {
		perror("inotify_add_watch");
		return EXIT_FAILURE;
	}

	while (1) {
		int length = read(fd, buffer, EVENT_BUF_LEN);
		if (length < 0) {
			perror("read");
			return EXIT_FAILURE;
		}

		for (int i = 0; i < length; ) {
			struct inotify_event *event = (struct inotify_event *) &buffer[i];
			time_t current_time = time(NULL);

			if (event->mask & IN_MODIFY) {
				if (difftime(current_time, last_event_time) >= debounce_time) {
					last_event_time = current_time;
					print_content(filepath);
				}
			}

			i += EVENT_SIZE + event->len;
		}
	}

	inotify_rm_watch(fd, wd);
	close(fd);
	return EXIT_SUCCESS;
}

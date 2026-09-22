#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RESET     "\033[0m"
#define BOLD      "\033[1m"
#define UNDERLINE "\033[4m"
#define BLINK     "\033[5m"
#define RED       "\033[31m"
#define GREEN     "\033[32m"
#define YELLOW    "\033[33m"
#define BLUE      "\033[34m"
#define MAGENTA   "\033[35m"
#define CYAN      "\033[36m"
#define WHITE     "\033[37m"

#define FMT_DATE  "%Y-%m-%d (%a)"
#define FMT_TIME  "%H:%M:%S"

int
sleep_until_next_second()
{
	struct timespec current_time;
	struct timespec sleep_time;

	clock_gettime(CLOCK_REALTIME, &current_time);

	sleep_time.tv_sec = current_time.tv_sec + 1;
	sleep_time.tv_nsec = 0;

	long remaining_nsec = sleep_time.tv_nsec - current_time.tv_nsec;
	if (remaining_nsec < 0) {
		remaining_nsec += 1e9;
	}

	struct timespec req = {0};
	req.tv_sec = 0;
	req.tv_nsec = remaining_nsec;

	return nanosleep(&req, NULL) == 0;
}

void
main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("%s TimeZone\n", argv[0]);
		return;
	}
	setenv("TZ", argv[1], 1);

	struct timespec ts = {.tv_sec = 1, .tv_nsec = 0};
	struct tm *tm;
	char date_buf[20], time_buf[20];

	printf("\033[?25l");
	do {
		printf("\033[2J\033[;1H");

		time_t now = time(NULL);

		tm = localtime(&now);
		strftime(time_buf, sizeof(date_buf), FMT_TIME, tm);
		strftime(date_buf, sizeof(time_buf), FMT_DATE, tm);
		printf(BOLD WHITE "  %s " CYAN "%s" RESET "\n\n",
			   date_buf, time_buf );

		fflush(stdout);
	} while (sleep_until_next_second());
}

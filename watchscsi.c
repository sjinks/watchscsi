#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>

static void check(const char* path)
{
	FILE* f   = fopen(path, "r+b");
	char* buf = NULL;

	if (!f) {
		perror("fopen");
		return;
	}

	if (fscanf(f, "%ms", &buf) != 1) {
		syslog(LOG_ALERT, "Error reading %s", path);
		fprintf(stderr, "Error reading %s\n", path);
		if (buf) {
			free(buf);
		}

		fclose(f);
		return;
	}

	if (!strcmp(buf, "offline\n")) {
		syslog(LOG_ALERT, "%s says the device is offline!", path);
		rewind(f);
		if (fprintf(f, "%s\n", "running") != 1) {
			syslog(LOG_ALERT, "Error writing to %s", path);
			fprintf(stderr, "Error writing to %s\n", path);
		}
	}

	free(buf);
	fclose(f);
}

int main(int argc, char** argv)
{
	size_t did_len;
	struct stat buf;
	char* path;
	int rc;

	if (argc < 2) {
		fprintf(stderr, "Usage:\n\twatchscsi device_id\n\nWhere device_id is the SCSI device ID (see /sys/bus/scsi/devices/)\n\n");
		return EXIT_FAILURE;
	}

	did_len = strlen(argv[1]);

	path = malloc(strlen("/sys/bus/scsi/devices//state") + did_len + 1);
	if (!path) {
		perror("malloc");
		return EXIT_FAILURE;
	}

	memcpy(path, "/sys/bus/scsi/devices/", strlen("/sys/bus/scsi/devices/"));
	memcpy(path + strlen("/sys/bus/scsi/devices/"), argv[1], did_len);
	memcpy(path + strlen("/sys/bus/scsi/devices/") + did_len, "/state\0", strlen("/state") + 1);

	rc = stat(path, &buf);
	if (-1 == rc) {
		perror("stat");
		free(path);
		return EXIT_FAILURE;
	}

	if (!S_ISREG(buf.st_mode)) {
		fprintf(stderr, "Fatal: '%s' is not a file\n", path);
		free(path);
		return EXIT_FAILURE;
	}

	openlog("watchscsi", LOG_CONS | LOG_PID, LOG_DAEMON);

	for (;;) {
		check(path);
		sleep(30);
	}

	free(path);
	closelog();
	return EXIT_SUCCESS;
}

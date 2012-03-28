/* picoproj - test program for libam7xxx
 *
 * Copyright (C) 2012  Antonio Ospite <ospite@studenti.unina.it>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @example examples/picoproj.c
 * A minimal example to show how to use libam7xxx to display a static image.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "am7xxx.h"

static void usage(char *name)
{
	printf("usage: %s [OPTIONS]\n\n", name);
	printf("OPTIONS:\n");
	printf("\t-f <filename>\t\tthe image file to upload\n");
	printf("\t-F <format>\t\tthe image format to use (default is JPEG)\n");
	printf("\t\t\t\tSUPPORTED FORMATS:\n");
	printf("\t\t\t\t\t1 - JPEG\n");
	printf("\t\t\t\t\t2 - NV12\n");
	printf("\t-l <log level>\t\tthe verbosity level of libam7xxx output (0-5)\n");
	printf("\t-p <power level>\tpower level of device, between %x (off) and %x (maximum)\n", AM7XXX_POWER_OFF, AM7XXX_POWER_TURBO);
	printf("\t\t\t\tWARNING: Level 2 and greater require the master AND\n\t\t\t\t\t the slave connector to be plugged in.\n");
	printf("\t-W <image width>\tthe width of the image to upload\n");
	printf("\t-H <image height>\tthe height of the image to upload\n");
	printf("\t-h \t\t\tthis help message\n");
	printf("\n\nEXAMPLE OF USE:\n");
	printf("\t%s -f file.jpg -F 1 -l 5 -W 800 -H 480\n", name);
}

int main(int argc, char *argv[])
{
	int ret;
	int exit_code = EXIT_SUCCESS;
	int opt;

	char filename[FILENAME_MAX] = {0};
	int image_fd;
	struct stat st;
	am7xxx_context *ctx;
	am7xxx_device *dev;
	int log_level = AM7XXX_LOG_INFO;
	am7xxx_power_mode power_mode = AM7XXX_POWER_LOW;
	int format = AM7XXX_IMAGE_FORMAT_JPEG;
	int width = 800;
	int height = 480;
	unsigned char *image;
	unsigned int size;
	am7xxx_device_info device_info;

	while ((opt = getopt(argc, argv, "f:F:l:p:W:H:h")) != -1) {
		switch (opt) {
		case 'f':
			strncpy(filename, optarg, FILENAME_MAX);
			break;
		case 'F':
			format = atoi(optarg);
			switch(format) {
			case AM7XXX_IMAGE_FORMAT_JPEG:
				fprintf(stdout, "JPEG format\n");
				break;
			case AM7XXX_IMAGE_FORMAT_NV12:
				fprintf(stdout, "NV12 format\n");
				break;
			default:
				fprintf(stderr, "Unsupported format\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'l':
			log_level = atoi(optarg);
			if (log_level < AM7XXX_LOG_FATAL || log_level > AM7XXX_LOG_TRACE) {
				fprintf(stderr, "Unsupported log level, falling back to AM7XXX_LOG_ERROR\n");
				log_level = AM7XXX_LOG_ERROR;
			}
			break;
		case 'p':
			power_mode = atoi(optarg);
			switch(power_mode) {
			case AM7XXX_POWER_OFF:
			case AM7XXX_POWER_LOW:
			case AM7XXX_POWER_MIDDLE:
			case AM7XXX_POWER_HIGH:
			case AM7XXX_POWER_TURBO:
				fprintf(stdout, "Power mode: %x\n", power_mode);
				break;
			default:
				fprintf(stderr, "Invalid power mode value, must be between %x and %x\n", AM7XXX_POWER_OFF, AM7XXX_POWER_TURBO);
				exit(EXIT_FAILURE);
			}
			break;
		case 'W':
			width = atoi(optarg);
			if (width < 0) {
				fprintf(stderr, "Unsupported width\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'H':
			height = atoi(optarg);
			if (height < 0) {
				fprintf(stderr, "Unsupported height\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'h':
			usage(argv[0]);
			exit(EXIT_SUCCESS);
			break;
		default: /* '?' */
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if (filename[0] == '\0') {
		fprintf(stderr, "An image file MUST be specified.\n");
		exit_code = EXIT_FAILURE;
		goto out;
	}

	image_fd = open(filename, O_RDONLY);
	if (image_fd < 0) {
		perror("open");
		exit_code = EXIT_FAILURE;
		goto out;
	}
	if (fstat(image_fd, &st) < 0) {
		perror("fstat");
		exit_code = EXIT_FAILURE;
		goto out_close_image_fd;
	}
	size = st.st_size;

	image = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, image_fd, 0);
	if (image == NULL) {
		perror("mmap");
		exit_code = EXIT_FAILURE;
		goto out_close_image_fd;
	}

	ret = am7xxx_init(&ctx);
	if (ret < 0) {
		perror("am7xxx_init");
		exit_code = EXIT_FAILURE;
		goto out_munmap;
	}

	am7xxx_set_log_level(ctx, log_level);

	ret = am7xxx_open_device(ctx, &dev, 0);
	if (ret < 0) {
		perror("am7xxx_open_device");
		exit_code = EXIT_FAILURE;
		goto cleanup;
	}


	ret = am7xxx_close_device(dev);
	if (ret < 0) {
		perror("am7xxx_close_device");
		exit_code = EXIT_FAILURE;
		goto cleanup;
	}

	ret = am7xxx_open_device(ctx, &dev, 0);
	if (ret < 0) {
		perror("am7xxx_open_device");
		exit_code = EXIT_FAILURE;
		goto cleanup;
	}

	ret = am7xxx_get_device_info(dev, &device_info);
	if (ret < 0) {
		perror("am7xxx_get_info");
		exit_code = EXIT_FAILURE;
		goto cleanup;
	}
	printf("Native resolution: %dx%d\n",
	       device_info.native_width, device_info.native_height);

	ret = am7xxx_set_power_mode(dev, power_mode);
	if (ret < 0) {
		perror("am7xxx_set_power_mode");
		exit_code = EXIT_FAILURE;
		goto cleanup;
	}

	ret = am7xxx_send_image(dev, format, width, height, image, size);
	if (ret < 0) {
		perror("am7xxx_send_image");
		exit_code = EXIT_FAILURE;
		goto cleanup;
	}

	exit_code = EXIT_SUCCESS;

cleanup:
	am7xxx_shutdown(ctx);

out_munmap:
	ret = munmap(image, size);
	if (ret < 0)
		perror("munmap");

out_close_image_fd:
	ret = close(image_fd);
	if (ret < 0)
		perror("close");

out:
	exit(exit_code);
}
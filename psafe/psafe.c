/*
 * psafe
 * https://erix.dev/etc/psafe
 *
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2024 Erik Junsved
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "psafe.h"

#define ERR_PREFIX "error: "
#define ERR_NULL_FORMAT ERR_PREFIX "failed to encode string: null format"
#define ERR_ENCODE ERR_PREFIX "failed to encode string: snprintf error code %d"
#define ERR_MALLOC ERR_PREFIX "failed to add newline to string: malloc error"
#define ERR_PRINT ERR_PREFIX "failed to print: string size %d, printf return %d"

static int expect_len(const char *format, va_list args) {
	va_list args_copy;
	va_copy(args_copy, args);
	return vsnprintf(NULL, 0, format, args_copy);
	va_end(args_copy);
}

static int eprint(const int newline, const char *format, va_list args) {
	if (format == NULL) {
		const int exp_internal = strlen(ERR_NULL_FORMAT);
		const int printed_internal = fprintf(stderr, ERR_NULL_FORMAT"\n");
		if (exp_internal != printed_internal) {
			return PSAFE_FATAL;
		}
		return PSAFE_ERROR;
	}

	const int expected = expect_len(format, args);
	if (expected < 0) {
		const int exp_internal = snprintf(NULL, 0, ERR_ENCODE"\n", expected);
		if (exp_internal < 0) {
			return PSAFE_FATAL;
		}
		const int printed_internal = fprintf(stderr, ERR_ENCODE"\n", expected);
		if (exp_internal != printed_internal) {
			return PSAFE_FATAL;
		}
		return PSAFE_ERROR;
	}

	if (!newline) {
		const int printed_internal = vfprintf(stderr, format, args);
		if (printed_internal != expected) {
			return PSAFE_FATAL;
		}
		return PSAFE_SUCCESS;
	}

	const int format_len = strlen(format);
	char* out_format = malloc(format_len + 2);
	if (out_format == NULL) {
		const int exp_internal = strlen(ERR_MALLOC);
		const int printed_internal = fprintf(stderr, ERR_MALLOC"\n");
		if (exp_internal != printed_internal) {
			return PSAFE_FATAL;
		}
		return PSAFE_ERROR;
	}

	strcpy(out_format, format);

	out_format[format_len] = '\n';
	out_format[format_len + 1] = '\0';

	const int printed_internal = vfprintf(stderr, out_format, args);
	free(out_format);

	if (printed_internal != expected + 1) {
		return PSAFE_FATAL;
	}
	return PSAFE_SUCCESS;
}

static int print(const int newline, const char *format, va_list args) {
	if (format == NULL) {
		const int retval = psafe_eprint(ERR_NULL_FORMAT"\n");
		return retval;
	}

	const int expected = expect_len(format, args);
	if (expected < 0) {
		const int retval = psafe_eprint(ERR_ENCODE"\n", expected);
		return retval;
	}

	if (!newline) {
		const int printed_internal = vprintf(format, args);
		if (printed_internal != expected) {
			const int retval =
				psafe_eprint(ERR_PRINT"\n", expected, printed_internal);
			return retval;
		}
		return PSAFE_SUCCESS;
	}

	const int format_len = strlen(format);
	char* out_format = malloc(format_len + 2);
	if (out_format == NULL) {
		const int retval = psafe_eprint(ERR_MALLOC"\n");
		return retval;
	}

	strcpy(out_format, format);

	out_format[format_len] = '\n';
	out_format[format_len + 1] = '\0';

	const int printed_internal = vprintf(out_format, args);
	free(out_format);

	if (printed_internal != expected + 1) {
		const int retval =
			psafe_eprint(ERR_PRINT"\n", expected, printed_internal);
		return retval;
	}
	return PSAFE_SUCCESS;
}

int psafe_print(const char *format, ...) {
	va_list args;
	va_start(args, format);
	const int retval = print(0, format, args);
	va_end(args);
	return retval;
}

int psafe_println(const char *format, ...) {
	va_list args;
	va_start(args, format);
	const int retval = print(1, format, args);
	va_end(args);
	return retval;
}

int psafe_eprint(const char *format, ...) {
	va_list args;
	va_start(args, format);
	const int retval = eprint(0, format, args);
	va_end(args);
	return retval;
}

int psafe_eprintln(const char *format, ...) {
	va_list args;
	va_start(args, format);
	const int retval = eprint(1, format, args);
	va_end(args);
	return retval;
}

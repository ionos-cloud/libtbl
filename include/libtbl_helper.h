/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "libtbl.h"
#include <ctype.h>	/* for isspace(); */
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>


/*
 * remove all spaces from the string @s in place
 */
void remove_spaces(char *s);

int contains(struct table_column *s, struct table_column **cs);

void escape_char(char *buf, char *str, const char escape, const char character);

int print_escaped_field(enum format_type pFormat, bool use_color, enum color pColor, char *str);

int print_table_fields_as_string(struct table_field *pFields,
				   struct table_column *pColumns,
				   bool use_color);

size_t get_dashed_line(char *buf, size_t buf_size, size_t len);

int print_table_field_as_string_escaped(struct table_field *pFields,
				   struct table_column *pColumns,
				   bool use_color,
				   enum format_type pFormat);
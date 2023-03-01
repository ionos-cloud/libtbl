// SPDX-License-Identifier: GPL-2.0-or-later
#include "libtbl.h"
#include "libtbl_helper.h"
#include <ctype.h>	/* for isspace(); */
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>


int contains(struct table_column *s, struct table_column **cs)
{
	int i = 0;

	while (cs[i]) {
		if (s == cs[i])
			return 1;
		i++;
	}

	return 0;
}

void escape_char(char *buf, char *str, const char escape, const char character)
{
	int i, j;

	for (i = 0, j = 0; str[i]; i++) {
		if (str[i] == character) {
			buf[j++] = escape;
		}
		buf[j++] = str[i];
	}

	buf[j] = '\0';
}

int print_escaped_field(enum format_type pFormat, bool use_color, enum color pColor, char *str)
{
	int ret;
	char *buf;

	switch(pFormat) {
	case FORMAT_CSV:
		buf = malloc((2 * strlen(str)) + 1);
		escape_char(buf, str, '"', '"');
		break;
	case FORMAT_JSON:
		buf = malloc((2 * strlen(str)) + 1);
		escape_char(buf, str, '\\', '"');
		break;
	default:
		return print_color(use_color, pColor, "\"%s\"", str);
	}

	ret = print_color(use_color, pColor, "\"%s\"", buf);
	free(buf);

	return ret;
}

int print_table_fields_as_string(struct table_field *pFields,
				   struct table_column *pColumns,
				   bool use_color)
{
	if (pColumns->m_type == FIELD_STR)
		return print_color(use_color, pFields->mColor, "\"%s\"", pFields->mName);
	else
		return print_color(use_color, pFields->mColor, "%s", pFields->mName);
}

size_t get_dashed_line(char *buf, size_t buf_size, size_t len)
{
	size_t count = 0;
	len = (len >= buf_size) ? buf_size - 1 : len;

	while (count < len)
		count += snprintf(buf + count, len - count + 1, "%s", "-");

	return count;
}

int print_table_field_as_string_escaped(struct table_field *pFields,
				   struct table_column *pColumns,
				   bool use_color,
				   enum format_type pFormat)
{
	if (pColumns->m_type == FIELD_STR)
		return print_escaped_field(pFormat, use_color, pFields->mColor, pFields->mName);
	else
		return print_color(use_color, pFields->mColor, "%s", pFields->mName);
}

static int print_table_fields_term(const char *prefix, struct table_field *pFields,
				 struct table_column **pColumns, bool use_color, int pWidth)
{
	int columnCount = 0;
	struct table_column *column = *pColumns;

	if (!column)
		return 0;

	print_color(use_color, pFields[columnCount].mColor,
		  (column->column_align == 'l') ? "%s%-*s" COLUMN_DELIMITER : "%s%*s" COLUMN_DELIMITER,
		  prefix ?: "", column->m_width - pWidth, pFields[columnCount].mName);

	for (column = *++pColumns, columnCount = 1; column; column = *++pColumns, columnCount++)
		print_color(use_color, pFields[columnCount].mColor, (column->column_align == 'l') ?
			  "%-*s" COLUMN_DELIMITER : "%*s" COLUMN_DELIMITER,
			  column->m_width, pFields[columnCount].mName);
	printf("\n");

	return 0;
}

static int print_table_fields_csv(struct table_field *pFields,
				struct table_column **pColumns, bool use_color)
{
	int columnCount;
	struct table_column *c = *pColumns;

	if (c)
		print_table_field_as_string_escaped(&pFields[0], c, use_color, FORMAT_CSV);

	for (c = *++pColumns, columnCount = 1; c; c = *++pColumns, columnCount++) {
		printf(",");
		print_table_field_as_string_escaped(&pFields[columnCount], c, use_color, FORMAT_CSV);
	}

	printf("\n");

	return 0;
}

/*FIXME: escape '"' in strings */
static int print_table_fields_json(const char *prefix, struct table_field *pFields,
				 struct table_column **pColumns, bool use_color)
{
	int columnCount;
	struct table_column *column = *pColumns;

	printf("%s{", prefix);

	if (column) {
		printf("\n%s\t\"%s\": ", prefix, column->m_name);
		if (!print_table_field_as_string_escaped(&pFields[0], column, use_color, FORMAT_JSON))
			print_color(use_color, pFields[0].mColor, "null");
	}

	for (column = *++pColumns, columnCount = 1; column; column = *++pColumns, columnCount++) {
		printf(",\n%s\t\"%s\": ", prefix, column->m_name);
		if (!print_table_field_as_string_escaped(&pFields[columnCount], column, use_color, FORMAT_JSON))
			print_color(use_color, pFields[columnCount].mColor, "null");
	}

	printf("\n%s}", prefix);

	return 0;
}

int print_table_fields_xml(const char *prefix, struct table_field *pFields,
				struct table_column **pColumns, bool use_color)
{
	int columnCount;
	struct table_column *column = *pColumns;

	for (column = *pColumns, columnCount = 0; column; column = *++pColumns, columnCount++) {
		printf("%s<%s>", prefix, column->m_name);
		print_table_fields_as_string(&pFields[columnCount], column, use_color);
		printf("</%s>\n", column->m_name);
	}

	return 0;
}

int print_table_fields(enum format_type pFormat, const char *prefix,
			    struct table_field *pFields, struct table_column **pColumns,
			    bool use_color, int pwidth)
{
	switch (pFormat) {
	case FORMAT_TERM:
		return print_table_fields_term(prefix, pFields, pColumns, use_color, pwidth);
	case FORMAT_XML:
		return print_table_fields_xml(prefix, pFields, pColumns, use_color);
	case FORMAT_CSV:
		return print_table_fields_csv(pFields, pColumns, use_color);
	case FORMAT_JSON:
		return print_table_fields_json(prefix, pFields, pColumns, use_color);
	default:
		return -EINVAL;
	}
}

/* Print @format in @pColor color*/
int print_color(bool use_color, enum color pColor, const char *format, ...)
{
	va_list args;
	int ret;

	va_start(args, format);
	if (use_color && (pColor != CNRM)) {
		ret = printf("%s", colors[pColor]);
		ret += vprintf(format, args);
		ret += printf("%s", colors[CNRM]);
	} else {
		ret = vprintf(format, args);
	}
	va_end(args);
	return ret;
}

int table_row_stringify(void *s, struct table_field *pFields,
			       struct table_column **pColumns, int humanize,
			       int prefix_len)
{
	int columnCount;
	size_t len;
	void *v;
	struct table_column *column;

	for (column = *pColumns, columnCount = 0; column; column = *++pColumns, columnCount++) {
		v = (void *)s + column->s_off + column->m_offset;

		if (column->m_tostr) {
			len = column->m_tostr(pFields[columnCount].mName, MAX_COLUMN_WIDTH,
					 &pFields[columnCount].mColor, v, humanize);
		} else {
			if (column->m_type == FIELD_NUM || column->m_type == FIELD_VAL)
				len = snprintf(pFields[columnCount].mName, MAX_COLUMN_WIDTH,
					       "%d",
					       *(int *)v);
			else if (column->m_type == FIELD_LLU)
				len = snprintf(pFields[columnCount].mName, MAX_COLUMN_WIDTH,
					       PRIu64,
					       *(uint64_t *)v);
			else
				len = snprintf(pFields[columnCount].mName, MAX_COLUMN_WIDTH,
					       "%s",
					       (char *)v);

			pFields[columnCount].mColor = column->clm_color;
		}

		if (!columnCount)
			len += prefix_len;

		if (column->m_width < len)
			column->m_width = len;
	}

	return 0;
}

int table_get_max_h_width(struct table_column **pColumns)
{
	int hdr_len;
	int max_hdr_len = 0;
	struct table_column *column;

	for (column = *pColumns; column; column = *++pColumns) {
		hdr_len = strlen(column->m_header);
		if (max_hdr_len < hdr_len)
			max_hdr_len = hdr_len;
	}

	return max_hdr_len;
}

void print_table_entry_term(const char *prefix, struct table_field *pFields,
				   struct table_column **pColumns, int hdr_width,
				   bool use_color)
{
	int columnCount;
	struct table_column *column;

	for (column = *pColumns, columnCount = 0; column; column = *++pColumns, columnCount++) {
		printf("%s%-*s" COLUMN_DELIMITER, prefix, hdr_width, column->m_header);
		print_color(use_color, pFields[columnCount].mColor, "%s\n", pFields[columnCount].mName);
	}
}

int print_table_single_row(void *v, enum format_type pFormat, const char *prefix,
			   struct table_column **pColumns, bool use_color, int humanize,
			   size_t prefix_len)
{
	struct table_field fields[MAX_COLUMN_COUNT];

	table_row_stringify(v, fields, pColumns, humanize, prefix_len);
	print_table_fields(pFormat, prefix, fields, pColumns, use_color, prefix_len);

	return 0;
}

int print_table_all_rows(void **v, enum format_type pFormat, const char *pre,
			   struct table_column **cs, bool use_color, int humanize,
			   size_t pre_len)
{
	int i, ret;

	for (i = 0; v[i]; i++) {
		if (i && pFormat == FORMAT_JSON)
			printf(",\n");
		ret = print_table_single_row(v[i], pFormat, pre, cs, use_color, humanize, pre_len);
		if (ret)
			return ret;
	}

	return 0;
}

int print_table_row_line(const char *prefix, struct table_column **pColumns,
				bool use_color, size_t prefix_len)
{
	struct table_field fields[MAX_COLUMN_COUNT];
	struct table_column *column;
	struct table_column **cs = pColumns;
	int columnCount;

	for (column = *cs, columnCount = 0; column; column = *++cs, columnCount++) {
		fields[columnCount].mColor = CNRM;
		if (column->m_type == FIELD_NUM)
			get_dashed_line(fields[columnCount].mName, MAX_COLUMN_WIDTH, column->m_width);
		else
			fields[columnCount].mName[0] = '\0';
	}

	print_table_fields(FORMAT_TERM, prefix, fields, pColumns, use_color, prefix_len);

	return 0;
}

bool table_contains_number(struct table_column **pColumns)
{
	for (int i = 0; pColumns[i]; i++)
		if (pColumns[i]->m_type != FIELD_STR && pColumns[i]->m_type != FIELD_VAL)
			return true;

	return false;
}

void table_delete_non_number_fields(struct table_field *pFields,
				   struct table_column **pColumns)
{
	struct table_column *column;
	int i;

	for (column = *pColumns, i = 0; column; column = *++pColumns, i++)
		if (column->m_type != FIELD_NUM) {
			pFields[i].mName[0] = '\0';
			pFields[i].mColor = CNRM;
		}
}

int print_table_header_term(const char *prefix, struct table_column **pColumns,
			    bool use_color, char align)
{
	struct table_column *column;
	char al = align;

	if (prefix && *prefix != '\0')
		print_color(use_color, *pColumns ? (*pColumns)->hdr_color : CNRM, "%s", prefix);

	for (column = *pColumns; column; column = *++pColumns) {
		if (align == 'a')
			al = column->column_align;

		if (al == 'c')
			print_color(use_color, column->hdr_color, "%*s%*s" COLUMN_DELIMITER,
				  (column->m_width + column->hdr_width) / 2, column->m_header,
				  (column->m_width - column->hdr_width + 1) / 2, "");
		else if (al == 'r')
			print_color(use_color, column->hdr_color, "%*s" COLUMN_DELIMITER,
				  column->m_width, column->m_header);
		else
			print_color(use_color, column->hdr_color, "%-*s" COLUMN_DELIMITER,
				  column->m_width, column->m_header);
	}
	printf("\n");

	return 0;
}

void print_table_header_csv(struct table_column **pColumns)
{
	struct table_column *column = *pColumns;

	if (column)
		printf("%s", column->m_name);

	for (column = *++pColumns; column; column = *++pColumns)
		printf(",%s", column->m_name);

	printf("\n");
}

/*
 * Find column with the name @name in the NULL terminated array
 * of columns @pColumns
 */
static struct table_column *table_find_column(const char *name,
					      struct table_column **pColumns)
{
	while (*pColumns) {
		if (!strcmp((*pColumns)->m_name, name))
			break;
		pColumns++;
	}

	return *pColumns;
}

/*
 * remove all spaces from the string @s in place
 */
void remove_spaces(char *s)
{
	char *new = s;

	while (*s) {
		if (!isspace(*s))
			*new++ = *s;
		s++;
	}
	*new = *s;
}

/*
 * Parse @delim separated list of fields @names to be selected and
 * add corresponding columns from the list of all columns @all
 * (NULL terminated array) to the array which should contain
 * the selection @sub (last element will be NULL)
 * @sub_len is the max number of elemnts in the target array @sub
 *
 * If parsing succeeds, returns 0.
 * -EINVAL is returned if corresponding column can't be found.
 */
static int table_select_columns(const char *names, const char *delim,
			 struct table_column **all,
			 struct table_column **sub,
			 int sub_len)
{
	char *name, *str;
	struct table_column *clm;
	int i = 0;

	str = strdup(names);
	if (!str)
		return -ENOMEM;

	remove_spaces(str);

	if (!strlen(str))
		return -EINVAL;

	name = strtok(str, delim);
	while (name && i < sub_len) {
		clm = table_find_column(name, all);
		if (!clm) {
			free(str);
			return -EINVAL;
		}
		sub[i++] = clm;
		name = strtok(NULL, delim);
	}

	sub[i] = NULL;

	free(str);

	return 0;
}

int table_column_count(struct table_column **pColumns)
{
	int i = 0;

	while (pColumns[i])
		i++;

	return i;
}

int table_extend_columns(const char *arg, const char *delim,
			 struct table_column **all,
			 struct table_column **cs,
			 int sub_len)
{
	struct table_column *sub[MAX_COLUMN_COUNT];
	const char *names = arg;
	int rc, i;

	if (*arg == '+' || *arg == '-')
		names = arg + 1;

	rc = table_select_columns(names, delim, all, sub, sub_len);
	if (rc)
		return rc;

	if (*arg == '-') {
		int k = 0;

		for (i = 0; cs[i]; i++)
			if (!contains(cs[i], sub))
				cs[k++] = cs[i];
		cs[k] = NULL;
	} else {
		if (*arg == '+')
			cs = &cs[table_column_count(cs)];

		for (i = 0; sub[i]; i++)
			cs[i] = sub[i];
		cs[i] = NULL;
	}

	return 0;
}

#define CLM_LST(m_name, m_header, m_type, tostr, align, h_color, \
		c_color, m_descr) \
	CLM(table_column, m_name, m_header, m_type, tostr, \
	    align, h_color, c_color, m_descr, sizeof(m_header), 0)

static int pstr_to_str(char *str, size_t len, enum color *pColor, void *v,
		int humanize)
{
	*pColor = 0;
	return snprintf(str, len, "%s", *(char **)v);
}

CLM_LST(m_name, "Field", FIELD_STR, pstr_to_str, 'l', CBLD, CNRM, "");
CLM_LST(m_header, "Header", FIELD_STR, NULL, 'l', CBLD, CNRM, "");
CLM_LST(m_descr, "Description", FIELD_STR, pstr_to_str, 'l', CBLD, CNRM, "");

static struct table_column *columnsList[] = {
	&clm_table_column_m_name,
	&clm_table_column_m_header,
	&clm_table_column_m_descr,
	NULL
};

/*
* Print table header and table for format TERM 
*/
int print_table_term(const char *prefix, struct table_column **pColumn, bool use_color)
{
	struct table_field *fields;
	int row = 0;
	int number_of_columns;

	number_of_columns = (sizeof(columnsList) / sizeof(*columnsList) -1);

	fields = malloc(sizeof(*fields) * number_of_columns * table_column_count(pColumn));
	if (!fields)
		return -ENOMEM;

	for (row = 0; pColumn[row]; row++)
		table_row_stringify((void *)pColumn[row],
				    fields + row * number_of_columns,
				    columnsList, true, 0);

	print_table_header_term(prefix, columnsList, use_color, 'a');

	for (row = 0; pColumn[row]; row++)
		print_table_fields_term(prefix, fields + row * number_of_columns,
				      columnsList, use_color, 0);
	free(fields);

	return 0;
}

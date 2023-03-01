/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef __H_TABLE
#define __H_TABLE

#include <stdbool.h>
#include <stdlib.h>

enum field_type {
	FIELD_STR,
	FIELD_VAL,
	FIELD_NUM,
	FIELD_LLU
};

enum format_type {
	FORMAT_TERM,
	FORMAT_CSV,
	FORMAT_JSON,
	FORMAT_XML
};

enum color {
	CNRM,
	CBLD,
	CUND,
	CRED,
	CGRN,
	CYEL,
	CBLU,
	CMAG,
	CCYN,
	CWHT,
	CDIM,
	CDGR,
	CSTRIKETHROUGH,
};

static const char * const colors[] = {
	[CNRM] = "\x1B[0m",
	[CBLD] = "\x1B[1m",
	[CUND] = "\x1B[4m",
	[CRED] = "\x1B[31m",
	[CGRN] = "\x1B[32m",
	[CYEL] = "\x1B[33m",
	[CBLU] = "\x1B[34m",
	[CMAG] = "\x1B[35m",
	[CCYN] = "\x1B[36m",
	[CWHT] = "\x1B[37m",
	[CDIM] = "\x1B[2m",
	[CDGR] = "\x1B[90m",
	[CSTRIKETHROUGH] = "\x1B[9m",
};

struct table_column {
	const char	*m_name;
	char		m_header[16];
	int		hdr_width;
	const char	*m_descr;
	enum field_type	m_type;
	int		m_width;
	unsigned long	m_offset;
	int		(*m_tostr)(char *str, size_t len, enum color *pColor,
				   void *v, int humanize);
	char		column_align;
	enum color	hdr_color;
	enum color	clm_color;
	unsigned long	s_off;	/* TODO: ugly move to an embedding struct */
};

#ifndef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif
#endif

#define _CLM(str, s_name, name, header, type, tostr, align, h_color, c_color, descr, width, off) \
	{ \
		.m_name		= s_name, \
		.m_header	= header, \
		.hdr_width	= sizeof(header) - 1, \
		.m_type		= type, \
		.m_descr	= descr, \
		.m_width	= width, \
		.m_offset	= offsetof(struct str, name), \
		.m_tostr	= tostr, \
		.column_align	= align, \
		.hdr_color	= h_color, \
		.clm_color	= c_color, \
		.s_off		= off \
	}

#define CLM(str, name, header, type, tostr, align, h_color, c_color, descr, width, off) \
struct table_column clm_ ## str ## _ ## name = \
	_CLM(str, #name, name, header, type, tostr, align, h_color, c_color, descr, width, off)

#define MAX_COLUMN_WIDTH 128
#define MAX_COLUMN_COUNT 50
#define COLUMN_DELIMITER "  "

struct table_field {
	char mName[MAX_COLUMN_WIDTH];
	enum color mColor;
};

/*
* Print @str in @pColor color.
*/
#define COLOR_STR(use_color, pColor, str) \
	trm ? colors[pColor] : "", str, use_color ? colors[CNRM] : ""

int table_row_stringify(void *s, struct table_field *pfields,
			struct table_column **pColumns, int humanize,
			int pre_len);

int table_get_max_h_width(struct table_column **pColumns);

bool table_contains_number(struct table_column **pColumns);

void table_delete_non_number_fields(struct table_field *pFields,
			    struct table_column **pColumns);

int table_extend_columns(const char *names, const char *delim,
			 struct table_column **all,
			 struct table_column **cs,
			 int sub_len);

int table_column_count(struct table_column **pColumns);

/*	@
* brief Print table for @format
* @param format JSON/XML/TERM/CSV 
* @param prefix 
* @param pFields 
* @param pColumns 
* @param use_color 
* @param pwidth 
* @return
*/ 
int print_table_fields(enum format_type format, const char *prefix,
		     struct table_field *pFields, struct table_column **pColumns,
		     bool use_color, int pwidth);

/* Print table header for format TERM */
int print_table_header_term(const char *prefix, struct table_column **pColumns,
			    bool use_color, char align);

/*
* Print table header and table for format TERM 
*/
int print_table_term(const char *prefix, struct table_column **pColumn,
			 bool use_color);

void print_table_entry_term(const char *prefix, struct table_field *pFields,
			    struct table_column **pColumns, int hdr_width, bool use_color);

/* CSV Print functions*/
void print_table_header_csv(struct table_column **pColumns);


/* Print @format in @pColor */
int print_color(bool use_color, enum color pColor, const char *format, ...);

int print_table_single_row(void *v, enum format_type format, const char *pre,
		    struct table_column **pColumns, bool use_color, int humanize,
		    size_t pre_len);

int print_table_all_rows(void **v, enum format_type format, const char *pre,
		    struct table_column **pColumns, bool use_color, int humanize,
		    size_t pre_len);

int print_table_row_line(const char *pre, struct table_column **pColumns,
			 bool use_color, size_t pre_len);

			 
#endif /* __H_TABLE */

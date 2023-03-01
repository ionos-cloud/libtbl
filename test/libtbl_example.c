// SPDX-License-Identifier: GPL-2.0-or-later
#include <errno.h>
#include <stdio.h>
#include <unistd.h>	/* for isatty() */
#include <string.h> /* for strcmp() */
#include "libtbl.h"

struct test {
	char *name;
	int age;
	int marks;
	char *country;
};

static const struct test a = {"Alice", 25, 90, "United Kingdom"};
static const struct test b = {"Bob", 28, 85, "Australia"};
static const struct test c = {"Charles", 23, 68, "Denmark"};
static const struct test d = {"Frank", 24, 70, "India"};

static const struct test * rows[] = {
	&a,
	&b,
	&c,
	&d,
	NULL
};

#define NUMBER_OF_ROWS (sizeof(rows) / sizeof(*rows) - 1)

#define CLM_TEST(m_name, m_header, m_type, tostr, align, h_color, c_color, \
		m_descr) \
	static CLM(test, m_name, m_header, m_type, tostr, align, h_color, \
	    c_color, m_descr, sizeof(m_header) - 1, 0)

static int pstr_to_str(char *str, size_t len, enum color *pColor, void *v,
		int humanize)
{
	*pColor = 0;
	return snprintf(str, len, "%s", *(char **)v);
}

static int i_to_str(char *str, size_t len, enum color *pColor, void *v,
		    int humanize)
{
	int i = *(int *)v;

	if (i > 1000)
		*pColor = CRED;
	else
		*pColor = CGRN;
	return snprintf(str, len, "%d", *(int *)v);
}

CLM_TEST(name, "Student Name", FIELD_STR, pstr_to_str, 'l', CBLD, CNRM,
	 "Name ");
CLM_TEST(age, "Age", FIELD_NUM, i_to_str, 'r', CBLD, CNRM,
	 "Age ");
CLM_TEST(marks, "Marks", FIELD_NUM, i_to_str, 'r', CBLD, CNRM,
	 "Marks ");
CLM_TEST(country, "Country", FIELD_STR, pstr_to_str, 'l', CBLD, CNRM,
	 "Country.");

static struct table_column *test_columns[] = {
	&clm_test_name,
	&clm_test_age,
	&clm_test_marks,
	&clm_test_country,
	NULL
};

#define NUMBER_OF_COLUMNS (sizeof(test_columns) / sizeof(*test_columns) - 1)

int is_terminal;

static void print_usage(char *prog)
{
	printf("usage:");
	print_color(1, CBLD, prog);
	printf(" [option]\n\nOptions:\n");
	printf("Select one of the following formats to print table: \n");
	printf(" terminal, json, xml, csv, help \n");
}

int main(int argc, char **argv)
{
	struct table_column **columns = test_columns;
	int i;

	is_terminal = (isatty(STDOUT_FILENO) == 1);

	if ((argc <= 1) || ((argc > 1) && (!strcmp("terminal", argv[1])))) {
		struct table_field fields[NUMBER_OF_ROWS * NUMBER_OF_COLUMNS];

		for (i = 0; rows[i]; i++)
			table_row_stringify((void *)rows[i], fields + i * NUMBER_OF_COLUMNS,
					    		columns, true, 0);

		print_table_header_term("", columns, is_terminal, 'a');

		for (i = 0; rows[i]; i++)
			print_table_fields(FORMAT_TERM, " ",
							fields + i * NUMBER_OF_COLUMNS,
					      	columns, CBLD, 0);
	} else if (!strcmp("help", argv[1])) {

		print_usage(argv[0]);

	} else if (!strcmp("table", argv[1])) {

		print_table_term("", columns, is_terminal);
		
	} else if (!strcmp("csv", argv[1])) {

		print_table_header_csv(columns);
		print_table_all_rows((void **)rows, FORMAT_CSV,
				  0, columns, 0, 0, 0);

	} else if (!strcmp("json", argv[1])) {

		printf("{\n\t\"rows\": [\n");
		print_table_all_rows((void **)rows, FORMAT_JSON,
				"\t\t", columns, 0, 0, 0);
		printf("\n\t]\n}\n");

	} else if (!strcmp("xml", argv[1])) {

		printf("<rows>\n");
		for (i = 0; rows[i]; i++) {
			printf("\t<columns>\n");
			print_table_single_row((void *)rows[i], FORMAT_XML,
					"\t\t", columns, 0, 0, 0);
			printf("\t</columns>\n");
		}
		printf("</rows>\n");

	} 
	else {
		print_usage(argv[0]);
		return -EINVAL;
	}

	return 0;
}

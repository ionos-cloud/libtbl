// SPDX-License-Identifier: GPL-2.0-or-later
#include <gtest/gtest.h>
#include <string.h>

using namespace std;

const int STRING_SIZE = 256;

extern "C"{
  #include "../include/libtbl.h"
  #include "../include/libtbl_helper.h"
}

TEST(LibtblUnitTests, RemoveSpaces) {
  char str[15] = "Hello World   ";
  char *result = "HelloWorld";
  remove_spaces(str);
  ASSERT_STREQ(str, result);
}

TEST(LibtblUnitTests, EscapeChar) {
  {
    char str[] = "/";
    char result[] = "//";
    char buf[STRING_SIZE];
    escape_char(buf, str, '/', '/');
    ASSERT_STREQ(buf, result);
  }

  {
    char str[] = "//";
    char result[] = "////";
    char buf[STRING_SIZE];
    escape_char(buf, str, '/', '/');
    ASSERT_STREQ(buf, result);
  }

  {
    char str[] = "/abc/";
    char result[] = "//abc//";
    char buf[STRING_SIZE];
    escape_char(buf, str, '/', '/');
    ASSERT_STREQ(buf, result);
  }

  {
    char str[] = "+abc+";
    char result[] = "/+abc/+";
    char buf[STRING_SIZE];
    escape_char(buf, str, '/', '+');
    ASSERT_STREQ(buf, result);
  }
}

TEST(LibtblUnitTests, PrintEscapedField)
{
  {
    char buf[STRING_SIZE];
    freopen("/dev/null", "a", stdout);
    setbuf(stdout, buf);
    char str[] = "abc\"def\"ghi";
    char result[] = "\"abc\"\"def\"\"ghi";
    print_escaped_field(FORMAT_CSV, false, CRED, str);
    buf[strlen(result)] = '\0';
    freopen ("/dev/tty", "a", stdout);
    ASSERT_STREQ(buf, result);
  }

  {
    char buf[STRING_SIZE];
    freopen("/dev/null", "a", stdout);
    setbuf(stdout, buf);
    char str[] = "abc\"def\"ghi";
    char result[] = "\x1B[31m\"abc\"\"def\"\"ghi\"\x1B[0m\x10";
    print_escaped_field(FORMAT_CSV, true, CRED, str);
    buf[strlen(result)] = '\0';
    freopen ("/dev/tty", "a", stdout);
    ASSERT_STREQ(buf, result);
  }

  {
    char buf[STRING_SIZE];
    freopen("/dev/null", "a", stdout);
    setbuf(stdout, buf);
    char str[] = "abc\"def\"ghi";
    char result[] = "\"abc\\\"def\\\"ghi";
    print_escaped_field(FORMAT_JSON, false, CRED, str);
    buf[strlen(result)] = '\0';
    freopen ("/dev/tty", "a", stdout);
    ASSERT_STREQ(buf, result);
  }

    {
    char buf[STRING_SIZE];
    freopen("/dev/null", "a", stdout);
    setbuf(stdout, buf);
    char str[] = "abc\"def\"ghi";
    char result[] = "\x1B[34m\"abc\\\"def\\\"ghi\"\x1B[0m\x10";
    print_escaped_field(FORMAT_JSON, true, CBLU, str);
    buf[strlen(result)] = '\0';
    freopen ("/dev/tty", "a", stdout);
    ASSERT_STREQ(buf, result);
  }
}

TEST(LibtblUnitTests, GetDashedLine)
{
  {
    char buf[STRING_SIZE];
    char result[] = "-";
    get_dashed_line(buf, 16, 1);
    ASSERT_STREQ(buf, result);
  }

  {
    char buf[STRING_SIZE];
    char result[] = "-----";
    get_dashed_line(buf, 16, 5);
    ASSERT_STREQ(buf, result);
  }

  {
    char buf[STRING_SIZE];
    char result[] = "---------------";
    get_dashed_line(buf, 16, 16);
    ASSERT_STREQ(buf, result);
  }

  {
    char buf[STRING_SIZE];
    char result[] = "---------------";
    get_dashed_line(buf, 16, 17);
    ASSERT_STREQ(buf, result);
  }
}

TEST(LibtblUnitTests, PrintColor)
{
  {
    char buf[STRING_SIZE];
    freopen("/dev/null", "a", stdout);
    setbuf(stdout, buf);
    char result[] = "\x1B[31mRED\x1B[0m";
    print_color(true, CRED, "RED");
    buf[strlen(result)] = '\0';
    freopen ("/dev/tty", "a", stdout);
    ASSERT_STREQ(buf, result);
  }

  {
    char buf[STRING_SIZE];
    freopen("/dev/null", "a", stdout);
    setbuf(stdout, buf);
    char result[] = "RED";
    print_color(false, CRED, "RED");
    buf[strlen(result)] = '\0';
    freopen ("/dev/tty", "a", stdout);
    ASSERT_STREQ(buf, result);
  }
}
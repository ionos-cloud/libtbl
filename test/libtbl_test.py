# SPDX-License-Identifier: GPL-2.0-or-later
import subprocess
import filecmp
import os
import argparse

def check_terminal_output(binary_path):
    with open('result.txt',"w") as outfile:
        subprocess.check_call([binary_path + "/libtbl_example", "terminal"],stdout=outfile)

        if filecmp.cmp('result.txt', 'expected_output/terminal_expected_output.txt'):
            print("Pass")
        else:
            print("Error: Compare expected_output/terminal_expected_output.txt and test/terminal_output.txt")
            os.rename('result.txt', 'terminal_output.txt')
            exit(1)

def check_json_output(binary_path):
    with open('result.txt',"w") as outfile:
        subprocess.check_call([binary_path + "/libtbl_example", "json"],stdout=outfile)

        if filecmp.cmp('result.txt', 'expected_output/json_expected_output.txt'):
            print("Pass")
        else:
            print("Error: Compare expected_output/json_expected_output.txt and test/json_output.txt")
            os.rename('result.txt', 'json_output.txt')
            exit(1)

def check_xml_output(binary_path):
    with open('result.txt',"w") as outfile:
        subprocess.check_call([binary_path + "/libtbl_example", "xml"],stdout=outfile)

        if filecmp.cmp('result.txt', 'expected_output/xml_expected_output.txt'):
            print("Pass")
        else:
            print("Error: Compare expected_output/xml_expected_output.txt and test/xml_output.txt")
            os.rename('result.txt', 'xml_output.txt')
            exit(1)

def check_csv_output(binary_path):
    with open('result.txt',"w") as outfile:
        subprocess.check_call([binary_path + "/libtbl_example", "csv"],stdout=outfile)

        if filecmp.cmp('result.txt', 'expected_output/csv_expected_output.txt'):
            print("Pass")
        else:
            print("Error: Compare expected_output/csv_expected_output.txt and csv_output.txt")
            os.rename('result.txt', 'csv_output.txt')
            exit(1)

def main():
    parser = argparse.ArgumentParser(prog='libtbl_test',
                                      description='Regression test for libtbl')
    parser.add_argument('-p', '--binary_path', required=True, help='Path to the libtbl_example binary')
    args = parser.parse_args()

    print("libtbl integration test")
    print("Check table output::")
    check_terminal_output(args.binary_path)

    print("Check JSON output::")
    check_json_output(args.binary_path)

    print("Check XML output::")
    check_xml_output(args.binary_path)

    print("Check CSV output::")
    check_csv_output(args.binary_path)

    exit(0)

if __name__ == "__main__":
    main()
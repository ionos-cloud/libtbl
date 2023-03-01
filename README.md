## libtbl
Libtbl supports printing tables on terminal if following formats:
**JSON, XML, CSV, TERMINAL**

## Building and Installing libtbl
Libtbl use Makefile. 
1. `make`
2. `make install`

## Testing
1. **make test** 
    It invokes Regression Testing. 
	It compares ouput with expected output from libtlbl_example.
	
2. **make libtbl_unittests**
	Unit test framework. You must have gtest installed at your local system.

## Example file
Run 'make test'. It will compile libtbl_example,that demonstrates how the
output will look for supported formats.
`./libtbl_example help` to see all options. 

## Additional notes
- Removing or adding particular fields to the table can be done using table_extend_columns().
If string (field name) passed to the function begins with '-' field will be removed, if it
begins with '+' field will be added.

## Contributors
	Grzegorz Prajsner <grzegorz.prajsner@ionos.com>
	Danil Kipnis <danil.kipnis@ionos.com>
	Supriti Singh <supriti.singh@ionos.com>
	Florian-Ewald Mueller <florian-ewald.mueller@ionos.com>
	Moritz Wagner <moritz.wagner@ionos.com>
	Vaishali Thakkar <vaishali.thakkar@ionos.com>
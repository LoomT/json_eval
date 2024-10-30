## About json_eval

Terminal application written in C++ that allows evaluating expressions on a JSON file.

### Building json_eval

You can build this project with a CMake-based build system: run an already-installed CMake on this source tree and build it.
CMake version required: 3.29

Only tested on Windows 11 with GCC compiler

#### Example
* `cmake -DCMAKE_BUILD_TYPE=Release -S path/of/repository/root -B path/of/repository/root/cmake-build-release`
* `cmake --build path/of/repository/root/cmake-build-release`
* Binary should be now somewhere in path/of/repository/root/cmake-build-release

### Usage
./json_eval \<json_file> \<expression>

\<json_file>: JSON file path

\<expression>: expression to evaluate on the JSON file

Alternative usage: ./json_eval -k \<json_file>  
This will parse the JSON file and keep the application open allowing multiple expressions to be 
evaluated one by one  
To exit this mode type -x

#### Current functionality

* Trivial JSON paths
* Expressions in the subscript operator []
* Intrinsic functions
  * min with one or more number arguments xor an array of numbers
  * max with one or more number arguments xor an array of numbers
  * size of array, object or string
* Number literals: integers or IEEE floating points
* Arithmetic binary operators +, -, *, / and ^ (power function for now)
* Parentheses for encapsulating binary operations
* Descriptive error messages for invalid expressions and JSON/expression mismatches

#### Examples

./json_eval test.json "a.b[1]"

./json_eval -k test.json  
a  
a.b  
a.b[1]  
-x

./json_eval ..\\test2.json "a.b[0] + a.b[ 1 ] * a.b[a.b[0] + a.b[1]][0] / (2^2 * 9 ^ 0.5 / 3 ^ (2-1)) - 0.5"

./json_eval test3.json "max(5, a.b, size(c))"

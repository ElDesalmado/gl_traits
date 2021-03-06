
	Once in a while there comes a necessity for a programmer to read and use command line arguments 
in some small command-line application. The easiest approach for such a task (any task, in fact)
would be to search for a good open-source library, where everything you might need is already
implemented and is available out-of-the-box. 
Needless to say, a lot of people prefer to write their own code, rather then utilize ready-to-use 
solutions for various reasons.
For example: 
- Your program is really small and you don't want to "waste" your time on:
1. searching for libraries;
2. setting up dependencies for your project;
3. learning how to do 2.

While those few reasons are very bad excuses for real projects and developers who value time and
are result-oriented, there are some justifiable ones:
1. You are learning or practising.
2. You want to write an article.

The reason for this article is to share some experience at and thoughts on writing a module which
porpose is to recieve command-line arguments and make them easily available for a module-user.

My intention is essentially to write all-in-one article or series of articles to cover the following:

1. Developing the module
- naive approach;
- analisis; 
- using OOP;
... (to be continued)
2. Use of CMake:
- to set up the initial project;
- to manage dependencies;
- to export your module;
- to test;

**CMake: set up your project**
I've learned about CMake rather late (about a year after starting to learn C++), and 
I encourage you not to repeat that mistake. This is one of the base things you need to 
get familiar with prior to indulging in learning C++.

(add ref links to download CMake)

*Project folder tree*
|- marconi (generated with acronymify.com. "Manager ARgument COmmaNd lIne")
|---CMakeLists.txt
|---app.cpp

For now it will suffice. *CMakeLists.txt* contains scripts for CMake to generate a 
Solution (.sln if you prefer Visual Studio) for a single executable.
```
cmake_minimum_required(VERSION 3.5 FATA_ERROR) 

project(marconi LANGUAGES CXX)

add_executable(app
	app.cpp
	)
```

// create folder for solution
To generate a solution via CMake using the command line (I use powershell)
'cmake -S ./ -B $build -A x64' 
where:
'-S' for source path;
'-B' for path to write generated files;
'-A' for configutation (x32 if not specified)

After generation is complete, open solution:
'cmake --open $build'


**Naive approach**

Every C++ program intended to run at OS-managed environment must have an entry-point.
Here we have a familiar function, that is invoked by an OS.

```C++
int main(int c, const char* argv[]) // or "char **argv", does not matter 
{
	...
	return 0;
}
```

These arguments for 'main' describe an array of null-terminated strings (nts), where
'int c' - number of 'const char\*' arrays in the array 'const *char argv[]'.
The latter is essentially an array of pointers to char pointers.
The first argument (AFAIK) is a path to the executable. That means, invoking your program
as './prog.exe' will lead to 
'const char* path = argv[0]; // c = 1'

For example, if we pass arguments like when we invoked cmake:
'./prog.exe -S "C:/My Sources/marconi" -B $builf -A x64'
our function *main* will recieve an array of 7 nts
```C++
#include <iostream>

int main(int argc, const char *argv[])
{
	// print all the arguments
	for (int i = 0; i != argc; ++i)
		std::cout << argv[i] << ' ';
	std::cout << std::endl;
	
	if (argc != 7)
		return -1;
		
	const char *pathExe 	= argv[0], 	// path to executable
			*argNameSource 	= argv[1], 	// "-S\0"
			*argValSource 	= argv[2],	// "C:/My Sources/marconi\0"
			*argNameOutDir 	= argv[3],	// "-B\0"
			*argValOutDir 	= argv[4],	// path to output directory
			*argNameConfig 	= argv[5],	// "-A\0"
			*argValConfig 	= argv[6];	// "x65\0"
			
	return 0;
}
```

*Debugging app invoked with command-line arguments*
Since CMake v.3.13 it is possible to set command arguments for a project to be invoked with
(for Visual Studio). '\' used to escape paranthesis and for multiline string
```
set(ARGUMENTS
	"-S \"C:/My Sources/marconi\" -B \"C:/builds/folder name\" -A x64"
	)	
	
set_target_properties(app
	PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED True
		VS_DEBUGGER_COMMAND_ARGUMENTS ${ARGUMENTS}
	)
```

Arter regenerating your solution (save CMakeLists.txt and rebuild ZERO_CHECK project)
you can run your app and get code 0 returned insted of -1 (since we actually pass those args
to our app).

As you might have noticed, each "word" preceeded by a whitespace will be passed separately
(a new 'const char\*' array will be used), unless a group of words is encapsulated
with parentheses "". 
"C:/My Sources/marconi" - one argument;
C:/My Sources/marconi - two arguments ("C:/My\0" and "Sources/marconi\0");


Of course, using the code from above is a terrible idea since it does not:
- check if the values represent legitimate arguments (not some garbage/no spelling mistakes);
- check if the arguments are in the right order (in case it matters);
- check if the provided arguments suffice to run your program;
- terminate your program in all those cases;
- show error message;
- show help message;

Let's then define the conditions for your app to run successfully:
1. Each argument must have a preceeding tag with a '-' prefix, like '-S' (for Source).
2. Each tag must be followed by an argument value, unless it has a default value.
3. The necesary arguments will be Source (-S) and Output (-B) directories
4. Additional arguments will be Configuration (-A) (x64/x32) and number of threads (-T).
5. All the arguments must be provided with a valid value. Additional ones are valid 
unless overriden.
6. Argument (-h or -help) need no value and if provided the app will stop parsing and 
show the help message.


*Parsing command arguments*
First, modify the previous version:
```C++

#include <string_view>
#include <vector>

constexpr const char tagSource[] = "-S",
tagOutput[] = "-B",
tagConfig[] = "-A",
tagThreads[] = "-T",
tagHelp1[] = "-H",
tagHelp2[] = "-Help";

bool IsHelp(std::string_view tag);
void PrintHelp();

int main(int argc, const char *argv[])
{
	std::vector<std::string_view> commands{ argv, std::next(argv, argc) };
	
	using CIter = std::vector<std::string_view>::const_iterator;
	CIter iCommand = std::next(commands.cbegin()); // 0 arg is exe path

	if (iCommand == commands.cend() ||
		IsHelp(iCommand->data()))
	{
		PrintHelp();
		return -1;
	}
	
	// parse commands...
	// check values...
	
	return 0;
}

bool IsHelp(std::string_view tag)
{
	static std::string_view help1{ tagHelp1 },
		help2{ tagHelp2 };

	return tag == help1 ||
		tag == help2;
}

void PrintHelp()
{
	std::cout << "Usage: ./app.exe " << tagSource << " [source path] " <<
		tagOutput << " [output path] " << tagConfig << " [configuration = x32] " <<
		tagThreads << " [threads = 1]\n" <<
		"./app.exe -h\nor\n./app.exe --help\nto print this message." << std::endl;
}
```

*STL containers are able to be constructed from 2 iterators representing a range.
Pointers are also considered to be iterators, so we eventually have just copied 
our array of const char\* to that vector. No copies (apart from pointers) are made, 
since std::string_view does not allocate any memory.*

Here we check for 2 conditions: if a help tag has been passed, or command line was empty.
In latter case, there is only one argument - path to the executable.

*std::next() and std::prev() correspondedly return next and previous iterators*

*Storing argument values*
```C++

// includes ...
#include <map>
#include <string>
#include <optional>

constexpr const char tagSource[] = "-S",
tagOutput[] = "-B",
tagConfig[] = "-A",
tagThreads[] = "-T",
tagHelp1[] = "-H",
tagHelp2[] = "-Help";

using TagStr = std::string_view;

// it will sort tags in alphabetic order
std::map<TagStr, std::string> clArgs
{
		{tagSource, std::string()},
		{tagOutput, std::string()},
		{tagConfig, std::string("x32")},
		{tagThreads, std::string("1")}
};

using RefString = std::reference_wrapper<std::string>;

std::optional<RefString> GetValue(const TagStr& tag)
{
	using Iter = std::map<TagStr, std::string>::iterator;
	if (tag.empty())
		return std::nullopt;

	Iter found = clArgs.find(tag);
	if (found == clArgs.cend())
		return std::nullopt;

	return std::reference_wrapper(found->second);
}

// *code, main(), etc*

```

We use 'std::map' to store predefined tags and defaulted values. Alongside with that, we
encapsulate value retrieving mechanism into the 'GetValue' function, which returns
'std::optional' with a wrapped reference to the string value withing the map. 
If the map does not contain a tag, 'std::nullopt' will be returned.

*Parsing loop*
As we mentioned earlier, CL arguments are expected to go in order
'-name Value -name2 "compound value" -name3 ...'. So for each iteration
we find an argument by tag and modify its value.
The loop will be broken if:
- an unrecognized tag will be passed;
- value is followd by value;
- no CL arguments left;

Starting conditions that we define for each iteation (for now):
- there are elements to be parsed (checked while checking for 'IsHelp')
- current argument is a tag (arg name)

```C++
int main(int argc, const char *argv[])
{
	// prev code ...
	
	// break loop if starting conditions have not been met for new iteration
	auto IsArgName = [](std::string_view str, bool * loop_broken = nullptr)
	{
		bool res = str[0] == '-';;
		if (loop_broken)
			*loop_broken = !res;
		return res;
	};
	
	bool loop_broken = false;

	// starting conditions: first command is an arg name
	while (IsArgName(*iCommand, &loop_broken) &&
		++iCommand != commands.cend())
	{
		static std::string_view tag;
		static std::optional<RefString> refargVal = std::nullopt;

		// next command is also an arg name. Leaving current arg's value empty/default
		if (IsArgName(*iCommand))
			continue;

		tag = *std::prev(iCommand);
		refargVal = GetValue(tag);

		if (!refargVal.has_value())
		{
			std::cerr << "Error: Unrecognized tag \"" << tag << "\"" << std::endl;
			PrintHelp();
			return -1;
		}

		std::string& argVal = *refargVal;
		argVal = *iCommand;
		if (++iCommand == commands.cend())
			break;
	}

	if (loop_broken)
	{
		std::cerr << "Error: expected argument name. Recieved: "
			<< *iCommand << std::endl;
		PrintHelp();
		return -1;
	}
	
	// check result
	
	return 0;
}
```

Here we define 'IsArgName' as lambda since we intend to use it for this loop's 
conditions only. Hence the other parts of a potential program does not need it.
There are two cases for 'IsArgName': to break the loop and to skip the iteration 
(in case an argument name is followed by another name instead of a value).
In the first case we modify 'loop_broken' to deduce if the loop has ended with errors.

*Check recieved arguments*
Once all the CL arguments have been processed, we need to validate all the values.
Currently, there are only 3 possible types of values:
- a path ('-S [source_dir] -B [output_dir]');
- an integer ('-T [threads]');
- a custom string ('-A [config=x32|x64]');

Here are two small functions to validate values:
```C++
bool ArgValidConfig(const std::string & val)
{
	static std::string x32{ "x32" },
		x64{ "x64" };
	return val == x32 ||
		val == x64;
}

bool ArgValidThreads(const std::string & val)
{
	static std::string msg{ "Invalid value for \"threads\" parameter: " };

	try
	{
		int threads = std::stoi(val);
		return threads >= 1 && threads < 11;
	}
	catch (const std::invalid_argument& e)
	{
		std::cerr << msg << e.what() << std::endl;
		return false;
	}
	catch (const std::out_of_range& e)
	{
		std::cerr << msg << e.what() << std::endl;
		return false;
	}
	catch (...)
	{
		std::cerr << msg << "unknown error." << std::endl;
		return false;
	}
}
```

For threads *we* check if the value is in range, and 'std::stoi'
checks if the input is valid.
We will not define any checks for `path` values (besides emptiness) in this module.

*Full code*
```C++

#include <iostream>

#include <string_view>
#include <optional>

#include <string>
#include <vector>
#include <map>

constexpr const char tagSource[] = "-S",
tagOutput[] = "-B",
tagConfig[] = "-A",
tagThreads[] = "-T",
tagHelp1[] = "-H",
tagHelp2[] = "-Help";

using TagStr = std::string_view;

// it will sort tags in alphabetic order
std::map<TagStr, std::string> clArgs
{
		{tagSource, std::string()},
		{tagOutput, std::string()},
		{tagConfig, std::string("x32")},
		{tagThreads, std::string("1")}
};

using RefString = std::reference_wrapper<std::string>;

std::optional<RefString> GetValue(const TagStr& tag);

bool IsHelp(std::string_view tag);
void PrintHelp();

bool ArgValidConfig(const std::string& val);
bool ArgValidThreads(const std::string& val);

int main(int argc, const char *argv[])
{
	std::vector<std::string_view> commands{ argv, std::next(argv, argc) };

	using CIter = std::vector<std::string_view>::const_iterator;
	CIter iCommand = std::next(commands.cbegin()); // 0 arg is exe path

	if (iCommand == commands.cend() ||
		IsHelp(iCommand->data()))
	{
		PrintHelp();
		return -1;
	}


	// break loop if starting conditions have not been met for new iteration
	auto IsArgName = [](std::string_view str, bool * loop_broken = nullptr)
	{
		bool res = str[0] == '-';;
		if (loop_broken)
			*loop_broken = !res;
		return res;
	};

	bool loop_broken = false;

	// starting conditions: first command is an arg name
	while (IsArgName(*iCommand, &loop_broken) &&
		++iCommand != commands.cend())
	{
		static std::string_view tag;
		static std::optional<RefString> refargVal = std::nullopt;

		// next command is also an arg name. Leaving current arg's value empty/default
		if (IsArgName(*iCommand))
			continue;

		tag = *std::prev(iCommand);
		refargVal = GetValue(tag);

		if (!refargVal.has_value())
		{
			std::cerr << "Error: Unrecognized tag \"" << tag << "\"" << std::endl;
			PrintHelp();
			return -1;
		}

		std::string& argVal = *refargVal;
		argVal = *iCommand;
		if (++iCommand == commands.cend())
			break;
	}

	if (loop_broken)
	{
		std::cerr << "Error: expected argument name. Recieved: "
			<< *iCommand << std::endl;
		PrintHelp();
		return -1;
	}
	
	bool validInput = true;

	for (const auto& t : clArgs)
	{
		static std::string msg{ "Error: invalid value for argument " },
			msgVal{ "Expected value: " };

		if (t.second.empty())
		{
			std::cerr << "Error: argument " << t.first << " is empty!" << std::endl;
			validInput = false;
			continue;
		}

		if (t.first == tagConfig && 
			!ArgValidConfig(t.second))
		{
			std::cerr << msg << t.first << ". Recieved: " << t.second <<
				std::endl << msgVal << "[x32|x64]" << std::endl;
			validInput = false;
			continue;
		}

		if (t.first == tagThreads &&
			!ArgValidThreads(t.second))
		{
			std::cerr << msg << t.first << ". Recieved: " << t.second <<
				std::endl << msgVal << "[1..10]" << std::endl;
			validInput = false;
			continue;
		}

	}

	if (!validInput)
		return -1;

	for (const auto& a : clArgs)
		std::cout << a.first << " " << a.second << std::endl;

	return 0;
}

std::optional<RefString> GetValue(const TagStr & tag)
{
	using Iter = std::map<TagStr, std::string>::iterator;
	if (tag.empty())
		return std::nullopt;

	Iter found = clArgs.find(tag);
	if (found == clArgs.cend())
		return std::nullopt;

	return std::reference_wrapper(found->second);
}

bool IsHelp(std::string_view tag)
{
	static std::string_view help1{ tagHelp1 },
		help2{ tagHelp2 };

	return tag == help1 ||
		tag == help2;
}

void PrintHelp()
{
	std::cout << "Usage: ./app.exe " << tagSource << " [source path] " <<
		tagOutput << " [output path] " << tagConfig << " [configuration = x32] " <<
		tagThreads << " [threads = 1]\n" <<
		"./app.exe -h\nor\n./app.exe --help\nto print this message." << std::endl;
}


bool ArgValidConfig(const std::string & val)
{
	static std::string x32{ "x32" },
		x64{ "x64" };
	return val == x32 ||
		val == x64;
}

bool ArgValidThreads(const std::string & val)
{
	static std::string msg{ "Invalid value for \"threads\" parameter: " };

	try
	{
		int threads = std::stoi(val);
		return threads >= 1 && threads < 11;
	}
	catch (const std::invalid_argument& e)
	{
		std::cerr << msg << e.what() << std::endl;
		return false;
	}
	catch (const std::out_of_range& e)
	{
		std::cerr << msg << e.what() << std::endl;
		return false;
	}
	catch (...)
	{
		std::cerr << msg << "unknown error." << std::endl;
		return false;
	}
}
```

*Analisis*
Though this code does what it was intended for, it is a terrible idea to use it anywhere
apart from this article/your example. 
Reasons:
1. Though there is some kind of encapsulation, it is not enough. Your 'main()' function
is about your main program, not about parsing CL arguments (however, this program is only
about parsing CL args), and it does not need to resemble spaghetti. 
If a function does not fit into your screen, it is a bad function©.
2. This code is not reusable. Everything that needs to copy-pasted is not reusable.
3. There are at least 3 responsibilities and 0 user-defined classes. See next article.
4. There is no way to modify the algorithm besides changing the code itself.
5. ... <your reasons>


**Testing with CMake**
Time to run some tests with various inputs.
Here is the modified 'CMakeLists.txt' version:

```
cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(marconi LANGUAGES CXX)

add_executable(app
	app.cpp
	)
	
set(ARGUMENTS
	"-S \"C:/My Sources/marconi\" -B \"C:/builds/folder name\" -A x64"
	)	

set_target_properties(app
	PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED True
		RUNTIME_OUTPUT_DIRECTORY bin
		VS_DEBUGGER_COMMAND_ARGUMENTS ${ARGUMENTS}
		
	)
	
enable_testing()

add_test(NAME TesValid1
	COMMAND 
		$<TARGET_FILE:app> 
		-S "C:/My Sources/marconi" 
		-B "C:/builds/folder name" 
		-A x64
	)
	
add_test(NAME TesValid2
	COMMAND 
		$<TARGET_FILE:app> 
		-S "C:/My Sources/marconi" 
		-B "C:/builds/folder name" 
	)
	
add_test(NAME TesValid3
	COMMAND 
		$<TARGET_FILE:app> 
		-B "C:/builds/folder name" 
		-S "C:/My Sources/marconi" 
	)
	
# -B specified twice
add_test(NAME TesFalseValid1
	COMMAND 
		$<TARGET_FILE:app> 
		-B "C:/builds/folder name" 
		-B "sdfsdgsdgdg" 
		-S "C:/My Sources/marconi" 
	)
```

'enable_testing()' command will add the 'RUN_TESTS' project to your Visual Studio solution.
Each 'add_test()' command will add a script to run our application 'app' with specified 
command line arguments. 'app' is an executable that we have built with 'add_executable',
'$<TARGET_FILE:<target_name>>' will be replaced with a path to it at configuration time.

To run all the tests, you need to *Build* the 'RUN_TESTS' project.
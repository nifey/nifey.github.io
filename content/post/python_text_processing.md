+++
title = "Notes: Text processing and Regular expressions in Python"
date = 2024-10-24
tags = ["python"]
draft = false
+++

In this post, we will learn by example, text processing and regular expression basics in Python.
We will also learn how to use inbuilt and external packages, to take command line arguments and to read data from files.

## Notes

### Execute python scripts like executable
Normally, we can execute python scripts by running `python script.py`. If we want to execute python scripts like an executable on the terminal we can add the following line as the first line of the script.
```python
#!/usr/bin/env python3
```
Add execute permissions on the script and now you can execute the script like an executable.
```sh
chmod +x file.py
./file.py
```

### Two ways to import from packages
1. Import the whole package and then use objects defined in it by using '.'
    ```python
    import sys
    print(sys.argv)
    ```

2. Import objects and functions selectively from packages
    ```python
    from sys import argv
    print (argv)
    ```

### Taking inputs via command line arguments
```python
from sys import argv
# Check argument count
if (len(argv) != 2):
    print("Usage: " + argv[0] + " <logfile>")
    exit()
filename = argv[1]
```

### File handling
We can open a file using `open` and close them with `close` functions like shown below:
```python
f = open("filename")
f = open("filename", "a") # Optionally specify mode: r (read) w (write), a (append), x (create)
data = f.read()
f.close()
```
However the common way is to use a `with` statement which will automatically close a file when execution exits the block.
```python
with open("filename", "r") as infile:
    # File processing
    infile.read()
    infile.readline()
    infile.readlines()
# File is automatically closed at this point
```

### Basic string processing
```python
line = "Hello world, Hi"

# Split a string into substrings delimited by the given character (here ',')
line.split(',')             # ["Hello world", " Hi"]
# Join the array elements with the given string as glue
"-".join(['a', 'b', 'c'])   # "a-b-c"

# Check if a string starts with a prefix, or ends with a suffix
line.startswith("Hello")    # True
line.startswith("World")    # False
line.endswith("Hi")         # True
line.endswith("Hello")      # False

# Convert string to lower or upper case
line.lower()
line.upper()

# Find the position of a substring in a string. Returns -1 if not found
line.find("world")          # 6
line.find("ABC")            # -1
```

### Regular expression basics
- Language for describing string patterns
    - used in grep, sed, awk, ...
    - used for find and replace, or searching
```
1. Specify characters to match
    a           matches character a
    [a-z]       matches lowercase alphabets
    [a-zA-Z0-9] matches alphabets and numbers
    [abc]       matches characters a b c
    [^abc]      matches anything except characters a b c

2. Specify count of characters to match
    *           Zero or more instances
    \+          One or more instances
    ?           Zero or one instances
    {8}         Matches 8 instances
    {1,3}       Matches 1 to 3 instances
    {3,}        Matches 3 or more instances

3. Special characters
    .           Matches any character
    ^           Matches the beginning of the line
    $           Matches the end of the line

4. Combining regex
    re1re2      Matches re1 first then re2 (Concatenation)
    re1|re2     Matches either re1 or re2 (Choice)

5. Backreferences
    Used for selecting a part of the matched string
    Enclose parts of the regex with ( .. ) parenthesis
    Can be referred as \1, \2, ... in re.sub
```

### Regular expression in Python
```python
import re

# Check for match only at beginning of string
m = re.match(r'pattern', 'string to search')
# Check for match anywhere in the string
m = re.search(r'pattern', 'string to search')
# Both of these functions return a match object
print(m.group(0)) # Gives the whole sequence that was matched
print(m.group(1)) # Gives the first matches subgroup (regex enclosed in parenthesis) if any
print(m.groups()) # Gives all matched subgroup (regex enclosed in parenthesis)

# Finds all non overlapping matches
re.findall(r'pattern', 'string to search')
# Find and replace with regular expression
re.sub(pattern, replacement_pattern, string)
```
### Using external packages from (Python Package Index) PyPI
```sh
# Create and use a virtual environment (to avoid messing with global packages)
python -m venv env
source env/bin/activate
deactivate # To come out of virtual environment

# Install package with pip (Run inside virtual environment)
# pip will install the package from Python Package Index (PyPI) along with any dependencies
pip install <packagename>

# Inorder to run your script on some other system,
# In your development virtual environment, record python dependencies in a file
pip freeze > requirements.txt

# On new system, create a virtual environment
python3 -m venv newenv
source newenv/bin/activate
pip install -r requirements.txt # Use the requirements.txt file to install all dependencies
python script.py
```

## Script to extract information from sudo logs

A common usecase for writing a python script is to extract information we need from structured logs, and regular expressions are a powerful tool for doing that.
As an example, we will write a script that converts sudo logs into a format that is easier to view in a table format. Let's say, we are only interested in finding (1) which user executed (2) what command and (3) when it was executed. The sudo logs can be obtained by running `sudo journalctl _COMM=sudo > logdata` on your terminal. The default format looks like below:
```
Aug 03 13:56:41 iserver sudo[2685498]:   xyzuser : TTY=pts/8 ; PWD=/home/xyzuser/ok ; USER=root ; COMMAND=/usr/bin/apt-get update
Aug 03 13:56:41 iserver sudo[2685498]: pam_unix(sudo:session): session opened for user root by xyzuser(uid=0)
Aug 03 13:56:44 iserver sudo[2685498]: pam_unix(sudo:session): session closed for user root
Aug 03 13:56:47 iserver sudo[2685979]:   xyzuser : TTY=pts/8 ; PWD=/home/xyzuser/ok ; USER=root ; COMMAND=/usr/bin/apt-get upgrade
Aug 03 13:56:47 iserver sudo[2685979]: pam_unix(sudo:session): session opened for user root by xyzuser(uid=0)
Aug 03 13:56:48 iserver sudo[2685979]: pam_unix(sudo:session): session closed for user root
Aug 03 13:57:02 iserver sudo[2686016]:   xyzuser : TTY=pts/8 ; PWD=/home/xyzuser/ok ; USER=root ; COMMAND=/usr/bin/apt-get upgrade -f
Aug 03 13:57:02 iserver sudo[2686016]: pam_unix(sudo:session): session opened for user root by xyzuser(uid=0)
Aug 03 13:57:03 iserver sudo[2686016]: pam_unix(sudo:session): session closed for user root
Aug 03 13:57:30 iserver sudo[2686075]:   xyzuser : TTY=pts/8 ; PWD=/home/xyzuser/ok ; USER=root ; COMMAND=/usr/bin/apt-get dist-upgrade
Aug 03 13:57:30 iserver sudo[2686075]: pam_unix(sudo:session): session opened for user root by xyzuser(uid=0)
Aug 03 13:57:52 iserver sudo[2686075]: pam_unix(sudo:session): session closed for user root
Aug 03 14:03:02 iserver sudo[2689098]:   xyzuser : TTY=pts/14 ; PWD=/home/xyzuser ; USER=root ; COMMAND=/usr/bin/apt-get install gawk bison gcc make wget tar -y
Aug 03 14:03:02 iserver sudo[2689098]: pam_unix(sudo:session): session opened for user root by xyzuser(uid=0)
Aug 03 14:03:03 iserver sudo[2689098]: pam_unix(sudo:session): session closed for user root
Aug 03 14:05:05 iserver sudo[2689399]:   xyzuser : TTY=pts/13 ; PWD=/home/xyzuser/ok ; USER=root ; COMMAND=/usr/bin/do-release-upgrade
Aug 03 14:05:05 iserver sudo[2689399]: pam_unix(sudo:session): session opened for user root by xyzuser(uid=0)
Aug 03 14:05:07 iserver sudo[2689399]: pam_unix(sudo:session): session closed for user root
Aug 03 14:07:29 iserver sudo[2689779]:   xyzuser : TTY=pts/0 ; PWD=/home/xyzuser ; USER=root ; COMMAND=/sbin/reboot now
Aug 03 14:07:29 iserver sudo[2689779]: pam_unix(sudo:session): session opened for user root by xyzuser(uid=0)
Aug 03 14:07:29 iserver sudo[2689779]: pam_unix(sudo:session): session closed for user root
```

The python script for extracting the information and printing on the terminal is shown below:
```python
#!/usr/bin/env python3
from sys import argv
import os
import re

# We check if the user specified the filename of the logs by checking if the length
# of command line arguments is 2. Note that the first element of argv is the command used
# to execute the script
if (len(argv) != 2):
    print("Incorrect number of arguments passed")
    print("Usage: " + argv[0] + " <logfile>")
    exit() # If arguments is not as expected, exit

# Check if the filename exists in the filesystem using os.path.exists function
if not os.path.exists(argv[1]):
    print("File doesn't exist")
    exit() # If not, exit

filename = argv[1]

# Regex pattern for the sudo logs where the subgroups are datetime, username and the command
matchpattern=r'(^[a-zA-Z]+ [0-9]+ [0-9:]+)[^:]+:[ ]+([a-zA-Z0-9]+).*COMMAND=(.*)'
# Read the file and process each line
with open(filename, "r") as infile:
    for line in infile.readlines():
        # Check if "COMMAND" is present in the line, if not continue
        if line.find("COMMAND") != -1:
            # Match the regex pattern with the line
            m = re.match(matchpattern, line)
            # Extract the subgroups
            date = m.group(1)
            name = m.group(2)
            command = m.group(3)
            # Print them in the required format
            print(f"{date} : {name} : {command}")
```

The output from the above script looks like:
```
Aug 03 13:56:41 : xyzuser : /usr/bin/apt-get update
Aug 03 13:56:47 : xyzuser : /usr/bin/apt-get upgrade
Aug 03 13:57:02 : xyzuser : /usr/bin/apt-get upgrade -f
Aug 03 13:57:30 : xyzuser : /usr/bin/apt-get dist-upgrade
Aug 03 14:03:02 : xyzuser : /usr/bin/apt-get install gawk bison gcc make wget tar -y
Aug 03 14:05:05 : xyzuser : /usr/bin/do-release-upgrade
Aug 03 14:07:29 : xyzuser : /sbin/reboot now
```

Now we will use a python package called prettytable to print this information as a table. Make sure to install prettytable package using pip.
```python
#!/usr/bin/env python3
from sys import argv
import os
import re

# We import colortable from the prettytable package, create an instance of PrettyTable object
from prettytable.colortable import ColorTable, Themes
table = ColorTable(theme=Themes.OCEAN)
# We specify field names for the table and left align the command column
table.field_names = ["Time", "User", "Command"]
table.align["Command"] = "l"
# Information about how to use prettytable can be found on the PyPI page : https://pypi.org/project/prettytable/

if (len(argv) != 2):
    print("Incorrect number of arguments passed")
    print("Usage: " + argv[0] + " <logfile>")
    exit()

if not os.path.exists(argv[1]):
    print("File doesn't exist")
    exit()

filename = argv[1]

matchpattern=r'(^[a-zA-Z]+ [0-9]+ [0-9:]+)[^:]+:[ ]+([a-zA-Z0-9]+).*COMMAND=(.*)'
with open(filename, "r") as infile:
    for line in infile.readlines():
        if line.find("COMMAND") != -1:
            m = re.match(matchpattern, line)
            date = m.group(1)
            name = m.group(2)
            command = m.group(3)
            # Add the data as a row to the table
            table.add_row([date, name, command[:30]])

# Print the constructed table on the terminal
print(table)
```

Now the output shows the information in a pretty table:
```
+-----------------+---------+--------------------------------+
|       Time      |   User  | Command                        |
+-----------------+---------+--------------------------------+
| Aug 03 13:56:41 | xyzuser | /usr/bin/apt-get update        |
| Aug 03 13:56:47 | xyzuser | /usr/bin/apt-get upgrade       |
| Aug 03 13:57:02 | xyzuser | /usr/bin/apt-get upgrade -f    |
| Aug 03 13:57:30 | xyzuser | /usr/bin/apt-get dist-upgrade  |
| Aug 03 14:03:02 | xyzuser | /usr/bin/apt-get install gawk  |
| Aug 03 14:05:05 | xyzuser | /usr/bin/do-release-upgrade    |
| Aug 03 14:07:29 | xyzuser | /sbin/reboot now               |
+-----------------+---------+--------------------------------+
```

- [Python documentations](https://docs.python.org) for documentation on inbuilt libraries
- [Python Package Index](https://pypi.org) for searching for libraries
- [How to use regular expressions in Python](https://docs.python.org/3/howto/regex.html)
- [PrettyTable package](https://pypi.org/project/prettytable/)

+++
title = "Notes on Sed"
date = 2023-08-12
tags = ["linux"]
draft = false
+++

# Sed
- Stream EDitor, UNIX utility
- Based on ed (line oriented text editor)
- Used commonly for **find and replace** based on Regular expressions
- Useful for processing and transforming logs
- Can also be used inside vim for find and replace
# Basic usage
```sh
cat file | sed 's/hello/world/'
sed 's/hello/world/' file
sed file -e 's/hello/world/'

sed 's/hello/world/' -i file	# Inline (will replace in the file)

cat file | sed '/REGEX/d'	# Delete lines matching a regular expression
```

# Regular expression syntax
- Language to represent string patterns
- Useful beyond sed (eg: grepping through source code)
- Basic regex: <characters to match><count of characters to match>
- Specify characters to match
  - **'a'** matches character a
  - **[a-z]** matches lowercase alphabets
  - **[a-zA-Z0-9]** matches alphabets and numbers
  - **[abc]** matches characters a b c
  - **[^abc]** matches anything except characters a b c
- Specify count of characters to match
  - **\*** -> Zero or more instances
  - **\\+** -> One or more instances
  - **?** -> Zero or one instances
  - **{8}** -> Matches 8 instances
  - **{1,3}** -> Matches 1 to 3 instances
  - **{3,}** -> Matches 3 or more instances
- Special characters
  - **.** -> Matches any character
  - **^** -> Matches the beginning of the line
  - **$** -> Matches the end of the line
- Combining regex
  - **'regex1regex2'** matches regex1 first then regex2 (Concatenation)
  - **'regex1\\|regex2'** matches either regex1 or regex2 (Choice)
- Backreferences
  - Used for selecting a part of the matched string to be used for transforming text
  - Enclose parts of the regex with \\( .. \\) parenthesis
  - Use the corresponding matches in replacement using \1, \2, etc to match the first, second backreference respectively.

# Sed command syntax
```
PATTERN_SPACE { commands; ...}
```
## Pattern space
- Selects (filters) the lines that we want to process
- Can be specified as line number ranges or regular expressions
```sh
sed '7d' file			# Delete line number 7
sed '1,10d' file		# Delete line number 1 to 10
sed '/REGEX/d' file		# Delete lines matching REGEX
sed '3,/REGEX/d' file		# Delete from 3rd line to a line matching REGEX
sed '/REGEX1/,/REGEX2/d' file	# Delete lines starting from a line matching REGEX1 to a line matching REGEX2
sed '/REGEX1/,$d' file		# Delete lines starting from a line matching REGEX1 till end of file
sed '/REGEX1/ {/REGEX2/d}' file # Delete lines matching both REGEX1 and REGEX2
```

## Substitute command for find and replace
```sh
# Finds patterns matching REGEX1 and replaces with REGEX2
cat file | sed 's/REGEX1/REGEX2/[flags]'
# Optional flags
# i to ignore case
# g to replace more than one instance of the pattern in a line
# c to confirm every replacement (Only available in vim mode)
```

# Other commands
```sh 
# Delete command
cat file | sed '/REGEX/d'	# Delete lines matching a regular expression

# Print command
cat file | sed -n '10p'		# Print 10th line
cat file | sed -n '/REGEX/p'	# Grep

# Transliterate command
cat file | sed 'y/abc/ABC/'	# Transliterate (a->A, b->B, c->C)
```

For more information, execute `info sed` on the shell to get a manual on sed.

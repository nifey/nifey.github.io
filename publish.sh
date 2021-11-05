#!/bin/bash
# This script will 
# - generate the static HTML files with Hugo
# - place the generated HTML in docs folder
# - commit the generated files
pretty_print() {
	echo -ne '\033[1;44mPublish script >>> '
	echo -n $1
	echo -e '\033[0;0m'
}

pretty_print "Generating static HTML using Hugo"
hugo --destination docs
if [ $? -eq 0 ]
then
	pretty_print "Successfully generated HTML"
	git add docs
	pretty_print "Commiting static files"
	git commit -m "Add generated files"
	pretty_print "Pushing to origin"
	git push origin master
else
	pretty_print "Hugo failed. Not commiting."
fi

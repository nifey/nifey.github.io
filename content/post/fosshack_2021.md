+++
title = "FOSSHack 2021"
date = 2021-12-04
tags = ["python", "hackathon", "ilugc"]
draft = false
+++

## FOSSHack 2021 {#fosshack-2021}

[FOSSHack 2021](https://fossunited.org/fosshack/2021) is a remote hackathon, organized by [FOSSUnited](https://fossunited.org/), that happened on 13-14 November.
FOSSUnited is a non-profit foundation that seeks to foster more FOSS projects and contributors from India and this hackathon is one such initiative.

The rules were simple: _Build a FOSS project or Work on existing FOSS project during the weekend._
The projects would be judged based on factors like technical complexity, completeness and utility.

I came to know about FOSSHack from the [ILUGC mailing list](https://www.freelists.org/post/ilugc/Fwd-BangPypers-Invitation-to-FOSS-Hack-2021).


### Team {#team}

The max team size was 4. So a subset of the [Smart India Hackathon](https://nihaal.me/post/2018-09-15-smart-india-hackathon-2018/) team: **Aswin, Aravindhan, Allwin and Me**, we participated as a team.
It has been 3 years since our first hackathon. We were studying B.E. then. Now we are all graduates.
All of us, except me, are working as software developers now. I'm still studying 😅.

We did not have any common programming language that we all knew well.
So we chose to use Python as it is easy to learn and has a lot of useful libraries.


### Idea {#idea}

In one of the [ILUGC](https://ilugc.in/) meets, [T Shrinivasan](https://goinggnu.wordpress.com/about/), who is a part of [FreeTamilEbooks](https://freetamilebooks.com/) and [Kaniyam Foundation](http://www.kaniyam.com/foundation/), was discussing about converting epub files of FreeTamilEbooks directly into HTML files for serving online.
He mentioned that it would be nice to convert the books into a [ReadTheDocs](https://readthedocs.org/)-like website for reading online.
There is also an [issue](https://github.com/KaniyamFoundation/ProjectIdeas/issues/70) on Kaniyam foundation's Project Ideas page about this.

We wanted to create a tool to solve this. A tool that converts epub files directly into interlinked HTML pages.
Reading about how ReadTheDocs generates documentation, we came to know about [Sphinx](https://www.sphinx-doc.org) documentation generator.
Sphinx converts ReST (Restructured text) files into beautiful HTML pages.

Instead of converting epub file to HTML directly, we thought of converting it first to ReST files
and then using Sphinx to generate HTML pages.
The advantage we get is that now we can use any of the available Sphinx themes.
The Sphinx generated HTML pages are also arranged like a book with sections displayed in the sidebar (in most themes).

So our task was to convert an epub file into ReST files and also generate config files (conf.py) for Sphinx.
Then we can use Sphinx to generate HTML.


## Epub2Sphinx {#epub2sphinx}


### [Epub](https://www.w3.org/publishing/epub32/) {#epub}

Epub is a widedy used, open standard format for Ebooks.
An epub file is actually a zip archive, in which the book text is stored as XHTML files.
It can also include images, CSS stylesheets, Fonts and other multimedia.

How do Epub readers know where to start reading?

-   Epub readers first read the _META-INF​/container.xml_ file to get the location of the OPF file.
-   The OPF (Open Package Format) file is an XML file that contains
    -   **Metadata**: title, author, language, license, etc
    -   **Manifest** : list of all items included in the epub, like documents, multimedia, CSS stylesheets and Fonts. Gives a mapping from _id_ to filename.
    -   **Spine** : list of _id_ references that tells us the linear reading order (i.e. in what order the files are shown to the reader).
    -   **Guide** : Additional information about the cover image, table of contents, etc.
-   Epub readers render the files according to the **Spine** and may use some information from the **Guide**.

More information about how ebook readers find the reading order from epub files can be found [here](http://apex.infogridpacific.com/df/ePubSpineOrder-packaging10.html).


### Tools and Libraries {#tools-and-libraries}

Epub2Sphinx is built on top of these tools and libraries:


#### 1. [Sphinx](https://www.sphinx-doc.org/) {#1-dot-sphinx}

Sphinx is a powerful documentation generator that is used by many Python projects and is also used for Linux kernel documentation.
It converts Restructured Text files into HTML pages.
It also has extensions that can generate documentation from Python docstrings.
Since it is widely used, it also has many good [themes](https://sphinx-themes.org) available.


#### 2. [Pandoc](https://pandoc.org/) {#2-dot-pandoc}

Pandoc is a universal markup convertor.
It is used for converting files between multiple document and markup language formats.
We used it to convert XHTML to ReST.

Pandoc can also convert epub to HTML directly, but it generates one big HTML file with almost no CSS styling (and so it does not look good).


#### 3. Python libraries {#3-dot-python-libraries}

-   [click](https://click.palletsprojects.com/) :
    Helps us create a neat CLI interface.
    We found [this article](https://medium.com/nerd-for-tech/how-to-build-and-distribute-a-cli-tool-with-python-537ae41d9d78) useful in getting upto speed, creating the CLI tool.
-   [ebooklib](https://github.com/aerkalov/ebooklib) :
    Library for parsing epub file.
    This library did most of the hard work of loading the content from the epub file.
    It provides good abstractions to work with the epub content.
-   [pypandoc](https://github.com/NicklasTegner/pypandoc) :
    Thin wrapper for Pandoc. Allows us to invoke Pandoc from Python.


### Core Functionality {#core-functionality}

The tool needs to find the XHTML content of each chapter in the normal reading order and convert them into separate ReST files.
The core functionality of the tool is this:

-   Get epub filename from user through CLI
-   Read the Epub using ebooklib
-   For each chapter (item) in the Spine (Linear reading order)
    -   Read the XHTML file
    -   Convert to ReST file using Pandoc
-   Generate conf.py for sphinx
-   Generate index.rst by filling the toctree

Initially we thought that converting HTML to ReST would be the hard part.
But turns out, Pandoc does it really well.
Pandoc did have issues when the tags were XML tags (defined by the Epub standard).
It skipped such tags. We had to do some hacks to convert some of those tags to HTML using regular expressions. For instance, some epubs had _<​image>_ tag which we converted to _<​img>_ tag for Pandoc to understand.

During the hackathon, we tested our tool with ebooks from [Project Gutenberg](https://www.gutenberg.org/) and  [FreeTamilEbooks](https://freetamilebooks.com/).
Thanks to the libraries and tools we built on, we were able to finish the core functionality during the hackathon.

Screenshots of _Pride and Prejudice_ book (from Project Gutenberg) converted using Epub2Sphinx, are shown below.

{{< figure src="/ox-hugo/epub2sphinx_front_page.png" >}}

{{< figure src="/ox-hugo/epub2sphinx_chapter_1.png" >}}


## Results {#results}

On 22nd, the hackathon [results](https://forum.fossunited.org/t/foss-hack-2021-results/957) were announced.
12 out of around 50 projects were selected as winners.
We were glad to see epub2sphinx on the list.

{{< figure src="/ox-hugo/epub2sphinx_results.png" >}}

We won Rs.25,000 for the project.
But more than that, we are happy that we built a FOSS tool that will be of use to people.
It was a weekend well spent.

We thank FOSSUnited volunteers for organizing this hackathon and T Shrinivasan for the idea.
I'm looking forward to participate in FOSSHack again next year.

The source code of Epub2Sphinx can be found [here](https://github.com/nifey/epub2sphinx).
Eventhough the core functionality is complete, Epub2Sphinx is still in development.
Contributions are welcome.

We recently published [Epub2Sphinx on PyPI](https://pypi.org/project/epub2sphinx/).
Do give it a try and share your thoughts.

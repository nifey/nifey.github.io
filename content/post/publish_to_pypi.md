+++
title = "Publishing packages to PyPI"
date = 2021-12-04
tags = ["python"]
draft = false
+++

The usability of a programming language depends on many factors and one of them is the availability of good libraries to build on.
When libraries are available, we also need a way to search and download libraries with ease.
Most modern langauges maintain some form of index to make dependency management easier. Examples include [Crates.io](https://crates.io/) for Rust and [PyPI](https://pypi.org/) for Python.

[Python Package Index (PyPI)](https://pypi.org) is an index and repository of Python packages.
Publishing your Python library or tool on PyPI allows any user to download it with **pip**.

[We](https://nihaal.me/post/fosshack%5F2021#team) recently published a package ([epub2sphinx](https://pypi.org/project/epub2sphinx/)) on PyPI.
I'm documenting the steps involved, in the hope that it will be useful for someone or just to me when I forget.


## Publishing packages to PyPI {#publishing-packages-to-pypi}


### 1. Setup your project {#1-dot-setup-your-project}

Before pushing to PyPI we have to package the project with **setuptools**.
To do that, we will add a **setup.py** file and some additional files.

-   Pick a package name that is not already used by some other package on PyPI.
    You can search for existing packages [here](https://pypi.org/search/).
    Don't wait till you make your project perfect to publish your package, because, since Python uses a linear namespace, your project name might get taken.
    Moreover, [Name squatting](https://www.reddit.com/r/rust/comments/86yr2x/python%5Fpep%5Fregarding%5Fpackage%5Fnames%5Fabandoned/) seems to be an issue on PyPI (and on Crates.io).
    If your project's basic functionality is working, go ahead and publish.
    You can always add more functionality in later releases.
-   Add a [LICENSE](https://choosealicense.com/) file
-   Add a README.md or README.rst file
-   Add a MANIFEST.in file.
    The MANIFEST.in file is used to specify what files (other than source code and README file) to include or exclude from the package. We will include the LICENSE in the package by including it in the MANIFEST.in file.
    ```nil
      include LICENSE
    ```
-   Add setup.py file.
    setup.py contains information about the package, its dependecies and more.
    The _setup()_ function takes these information as paramters.
    A template setup.py file is shown below.
    It uses the contents of README.md as the _long\_description_ and the contents of requirements.txt file as _install\_requires_ (i.e. dependencies).
    ```python
      from setuptools import setup, find_packages

      with open("README.md", "r", encoding="utf-8") as fh:
          long_description = fh.read()
      with open("requirements.txt", "r", encoding="utf-8") as fh:
          requirements = fh.read()

      setup(
          name = '<package_name>',
          version = '0.0.1',
          author='<author name>',
          author_email='<author email>',
          license = '<license>',
          description = '<short description>',
          long_description = long_description,
          long_description_content_type = "text/markdown",
          url = '<link to package repository>',
          project_urls={
              "Bug Tracker": "<bug tracker page>",
          },
          classifiers=[
              "Programming Language :: Python :: 3",
              "<Other classifiers>",
          ],
          packages = find_packages(),
          install_requires = [requirements],
      )
    ```
    -   Classifiers make it easy for people to search on PyPI.
        You can think of them as tags or categories that is used to filter down packages when searching on PyPI.
        You can find a list of all classifiers at <https://pypi.org/classifiers/>.
    -   If the app contains CLI tools, we can expose them as console scripts in [_entry\_points_](https://packaging.python.org/guides/distributing-packages-using-setuptools/#entry-points).
    -   If you want additional package files (other than code) to be included in the package, set the [_package\_data_](https://packaging.python.org/guides/distributing-packages-using-setuptools/#package-data) argument.
    -   If you are using ReST format for the Readme file, set _long\_description\_content\_type_ to "text/x-rst".

Detailed information about the arguments of _setup()_ can be found [here](https://packaging.python.org/guides/distributing-packages-using-setuptools/#setup-args).


### 2. Build {#2-dot-build}

Once we have the project set up, we can now generate the source distribution and the wheel file using setuptools.
_Source distribution_ and _Wheel file_ are just two different ways of providing the package to the user.
In the case of Source distribution, the source code of our package is downloaded and compiled at the user side, whereas the wheel file is already compiled and ready to install.

We can generate files for both distribution with the following command

```bash
python setup.py sdist bdist_wheel
```

This will generate package files under _dist_ directory.

```bash
ls dist
# package-0.0.1-py3-none-any.whl
# package-0.0.1.tar.gz
```


### 3. Upload to PyPI {#3-dot-upload-to-pypi}

-   Create an account on [PyPI](https://pypi.org/account/register/)
    Only registered users can upload packages to PyPI.
-   Install twine
    Twine is a utility tool for publishing on PyPI.
    It can be installed using pip.
    ```bash
      pip install twine
    ```
-   Twine check
    Twine can check and report common packaging mistakes. We can run the _check_ command on the distrubution files that we generated.
    ```bash
      twine check dist/*
    ```
-   Create a GPG key, if you don't have one already
    I'm not sure if this is required. But when I tried to upload a package to PyPI, it did prompt for a GPG key.
    To create a key, Install GPG and execute the following command.
    It will prompt for name, email, etc and create a GPG key.
    ```bash
      gpg --full-generate-key
    ```
-   Test it with [testpypi](https://test.pypi.org/)
    Before pushing your package to PyPI you can test it on testPyPI.
    This allows us to see how it would look like on the PyPI site without affecting the main index.
    Note that testpypi is a separate instance and so you have to create a new account on testpypi before pushing packages to it.
    To upload the package to testpypi, run:
    ```bash
      twine upload -r testpypi dist/*
    ```
    The -r flag specifies the repository to upload to, which is PyPI by default.
-   Upload
    ```bash
      twine upload dist/*
    ```
    The above command will prompt for your PyPI username and password, and then upload the package files under _dist_ directory to PyPI.

Congratulations! Your package should now be available on PyPI.


## Publishing new releases {#publishing-new-releases}

To publish newer versions of your package:

1.  Update _version_ field in setup.py

    We can do this manually by editing setup.py or use a tool called [_bump_](https://pypi.org/project/bump/).

    ```bash
       pip install bump
       # To change 1.2.3 to 2.0.0
       bump --major --reset
       # To change 1.2.3 to 1.3.0
       bump --minor --reset
       # To change 1.2.3 to 1.2.4
       bump --patch
    ```
2.  Remove any old package files from the _dist_ directory and build your package again

    ```bash
       rm dist/*
       python setup.py sdist bdist_wheel
    ```
3.  Check and upload new package files to PyPI

    ```bash
       twine check dist/*
       twine upload dist/*
    ```
4.  Create version tag

    If you are using git, it is important to create version tags. This lets us find which commits are included in a given version of the package.

    ```bash
       git tag -am "Release version 0.0.1" v0.0.1
       # Or if you prefer writing the tag message in your editor
       git tag -a v0.0.1

       # Push tags to remote
       git push --tags
    ```

    When creating a new release, it is good practice to summarize the changes between the old version and the new version in the tag message.

If you use Duckduckgo, you can use [**!pypi** bang](https://duckduckgo.com/bang?q=pypi) to search for a package in PyPI.


## References {#references}

-   [Packaging Python projects](https://packaging.python.org/tutorials/packaging-projects/)
-   [Distrubuting packages with setuptools](https://packaging.python.org/guides/distributing-packages-using-setuptools)

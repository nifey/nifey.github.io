+++
title = "Updating vulnerable Python dependencies"
date = 2021-10-18
tags = ["python"]
draft = false
+++

## Finding vulnerable dependencies {#finding-vulnerable-dependencies}

[Safety-db](https://github.com/pyupio/safety-db) is a database that keeps track of vulnerable python packages and version information.
It is updated once a month.
They also provide a tool called safety, that checks if the installed packages or packages in requirements.txt
are identified as vulnerable using the safety-db.

-   Install safety

    ```bash
    pip install safety
    ```
-   Use safety to check all packages in the current virtual enviroment

    ```bash
    safety check
    ```
-   Use safety to only check the dependencies listed in requirements.txt file

    ```bash
    safety check -r requirements.txt
    ```

Safety shows a list of python packages that have a known vulnerability.

```txt
+==============================================================================+
|                                                                              |
|                               /$$$$$$            /$$                         |
|                              /$$__  $$          | $$                         |
|           /$$$$$$$  /$$$$$$ | $$  \__//$$$$$$  /$$$$$$   /$$   /$$           |
|          /$$_____/ |____  $$| $$$$   /$$__  $$|_  $$_/  | $$  | $$           |
|         |  $$$$$$   /$$$$$$$| $$_/  | $$$$$$$$  | $$    | $$  | $$           |
|          \____  $$ /$$__  $$| $$    | $$_____/  | $$ /$$| $$  | $$           |
|          /$$$$$$$/|  $$$$$$$| $$    |  $$$$$$$  |  $$$$/|  $$$$$$$           |
|         |_______/  \_______/|__/     \_______/   \___/   \____  $$           |
|                                                          /$$  | $$           |
|                                                         |  $$$$$$/           |
|  by pyup.io                                              \______/            |
|                                                                              |
+==============================================================================+
| REPORT                                                                       |
| checked 63 packages, using free DB (updated once a month)                    |
+============================+===========+==========================+==========+
| package                    | installed | affected                 | ID       |
+============================+===========+==========================+==========+
| pillow                     | 8.3.1     | >=5.2.0,<8.3.2           | 41271    |
| pillow                     | 8.3.1     | >=7.1.0,<8.3.2           | 41277    |
| pip                        | 20.3.4    | <21.1                    | 40291    |
+==============================================================================+
```


## Updating dependencies {#updating-dependencies}

To update the dependencies we can use the pip-upgrade tool.
pip-upgrade updates the dependency package to the latest version and also updates the requirements.txt file.

-   Install pip-upgrader

    ```bash
    pip install pip-upgrader
    ```
-   Use pip-upgrader on a requirements.txt file

    ```bash
    pip-upgrade requirements.txt
    ```

    If the requirements.txt file is not provided, it expects to find it in the current directory.
-   Use pip-upgrader to update a specific package

    ```bash
    pip-upgrade requirements.txt -p pillow
    ```

{{% admonition tip "Check for regressions" %}}
Newer versions of dependencies may change the API that the application uses.
So when updating dependencies, It is always a good idea to check if the application works correctly with the updated packages.
{{% /admonition %}}


## References {#references}

-   [Safety-db list](https://pyupio.github.io/safety-db/)
-   [Safety-db on Github](https://github.com/pyupio/safety-db)
-   [Safety tool](https://pypi.org/project/safety/)
-   [Pip-upgrade tool](https://pypi.org/project/pip-upgrader/)

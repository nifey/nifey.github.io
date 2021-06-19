+++
title = "Backing up files with RSync"
date = 2021-06-19T11:16:00+05:30
tags = ["backup"]
draft = false
+++

## You need a backup {#you-need-a-backup}

When was the last time you took a backup of your files?
Thanks to technology, today, we can store thousands of photos and videos in a single pen drive.
Digital files are easy to store, but they are also easy to lose.
And that is why we all need to take regular backup of our files.


### 3-2-1 Backup Strategy {#3-2-1-backup-strategy}

3-2-1 Backup strategy is a general rule of thumb that says it is better to have atleast 3 copies of our files.

1.  **Working copy**
    -   This is the copy that your work on and have on your goto machine.
2.  **Local backup**
    -   This is a backup of data present in a local machine or an external hard disk.
    -   Having a local backup allows us to restore the files quickly in case the working copy is not accessible.
3.  **Remote backup**
    -   If both of your local copies become inaccessible, then you can still get your data from a remote backup, if you have one.

The best way to ensure that your data is safe, is to create an automatic system that takes
regular backups of your files.
Such a system should perform backups automatically based on a schedule
or based on a trigger (for example, when you plug in your external hard disk).

I'm trying to create such a backup system for myself.
As I make progress, I'll share what I learn in putting together such a system in this blog.

In this blog post, I'll discuss a backup tool called RSync.


## RSync {#rsync}

RSync is a popular commandline utility that is used to backup files.
It is written in C and is suppoted on many OSes including Linux (of course), Windows, OS X and BSD.
RSync also has many GUI frontends available. Some of them are GSync, QSync, Zynk, etc.
The general format of the command is shown below.

```bash
rsync [options] /path/to/source/folder /path/to/backup/folder
```


## When to use RSync {#when-to-use-rsync}

RSync tries to transfer only the difference between the current file and the file in the backup folder.
If the file does not exist in the backup folder, it copies the whole file.
This is especially helpful for incremental backups over a network i.e. when we want the modifications we made to a remote folder
to be synchronized with an existing backup of that folder.
RSync uses a diff based transfer algorithm, that splits a file into blocks and
transfers only the modified blocks to save bandwidth.
RSync also works over ssh.

RSync is best suited for unidirectional file synchronization, i.e. when we don't expect changes made
to the backup folder to be reflected in the source folder.


## When not to use RSync {#when-not-to-use-rsync}


### Bidirectional File synchronization {#bidirectional-file-synchronization}

RSync cannot be used easily for bidirectional transfers, where we may want to have modifications
in one copy get updated in the other copy and vice versa.
The problem is that if you modify a file in both the copies and then use RSync, one of them will be lost.
In other words, RSync does not resolve file conflicts.
In such a scenario, [Syncthing](https://syncthing.net/) is a better tool.


### Sometimes RSync wastes bandwidth {#sometimes-rsync-wastes-bandwidth}

RSync does not keep track of file moves and renames. So if we move or rename a file, it will treat it as a new file and copy it again.
Though this does not lead to data loss, it wastes bandwidth, especially if the size of the moved files are large.
This also means that if we do a file reorganization within the source folder, backing up with RSync will take longer due to unnecessary transfers.

Some workarounds to prevent RSync from making unnecesary file transfer, when the files were just renamed or moved, are:

-   _-_-fuzzy (-y) option, detects renames within a directory and avoids file transfer.
-   [Using hard links](https://lincolnloop.com/blog/detecting-file-moves-renames-rsync/)

The diff based algorithm also does not work well for compressed files, so RSync may copy compressed files fully even if only a small change was made.


## Things to note {#things-to-note}

-   **RSync has different behaviour with and without trailing backslash.**
    If the source folder is suffixed with a backslash, the contents of the folder are synchronized,
    whereas if we don't use the backslash after the source folder, the folder as a whole, is synchronized.
    It is always a better idea to use the dry run (-n) option to verify if RSync does what
    you expect it to do.
-   **Databases need to be dumped to files for RSync to backup.**
    RSync only works on files. So if you need to backup a database, you have to dump it
    to a file first. Most DBMS software have tools to dump the database contents to a file.
    One example is mysqldump for MySQL.
-   **On Linux, we can use crontab or SystemD timers to automate backups.**
-   **Always do a dry run before executing (especially when using _-_-delete)**


## Freqently used options {#freqently-used-options}

Here is a table of the most commonly used RSync options.

| Option                | Used for                                                                   |
|-----------------------|----------------------------------------------------------------------------|
| -a                    | preserves permission and other attributes                                  |
| -r                    | to copy recursively                                                        |
| -z                    | transfers data in compressed format (to save bandwidth)                    |
| -vhP                  | verbose, human-readable, show progress                                     |
| -e ssh                | transfer over ssh                                                          |
| -e 'ssh -p port'      | transfer over ssh using a different port                                   |
| -W                    | whole file transfer (instead of diff based)                                |
| _-_-dry-run, -n       | does not modify filesystem                                                 |
| _-_-delete            | removes files (in backup folder) that does not exist at the source  folder |
| _-_-exclude=""        | to specify which files to exclude                                          |
| _-_-exclude-from=file | exclude glob patterns specified in a file                                  |


## Common uses {#common-uses}


### Full system backup ([For more information](https://www.linuxsecrets.com/archlinux-wiki/wiki.archlinux.org/index.php/Full%5Fsystem%5Fbackup%5Fwith%5Frsync.html)) {#full-system-backup--for-more-information}

We can backup the whole Linux system with RSync.
The following command backs up programs (/bin), libraries (/lib), boot files (/boot),
and data (/home) to the backup folder.

```bash
sudo rsync -aAXv \
--exclude={"/dev/*","/proc/*","/sys/*","/tmp/*",\
           "/run/*","/mnt/*","/media/*","/lost+found"} \
/ /path/to/backup/folder
```

We exclude files in mounted partitions (/mnt/\*), directories populated at runtime (/proc/\*,/sys/\*) and filesystem specific folders like lost+found.
The -A option syncs the Access Control Lists of the files and -X option syncs the extended attributes of the files.


### Backup remote folder using ssh {#backup-remote-folder-using-ssh}

To backup over ssh, we simply add "-e ssh" to the options.

```bash
rsync -ave ssh /path/to/source/folder user@remote:~/backup/folder
```


### Incremental backup {#incremental-backup}

When used with the _-_-delete option, we can make the backup folder to be an exact replica of the source folder.

```bash
rsync -arvhP --delete /path/to/source/folder /path/to/backup/folder
```

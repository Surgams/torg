# torg

**A lightweight file organization tool for batch copying and renaming files across directory hierarchies.**

## Overview

**torg** is a small, efficient utility written in ANSI C99 that helps you organize files by copying and renaming them across nested directory structures. It preserves the original directory hierarchy while applying consistent naming conventions to the copied files, and optionally generates a mapping file to track original-to-new name associations.


---

## Installation

### Build Instructions

```sh
make all              # Build the project
make clean            # Remove compiled files
sudo make install     # Install the executable
sudo make deinstall   # Uninstall the executable

```
### Usage

```sh
torg [OPTIONS]

Options:
  -b, --base <BASE_FOLDER>         Path to the source directory
  -d, --dest <DESTINATION_FOLDER>  Path to the destination directory
  -f, --conf <CONF_FILE>           Path to configuration file
  -h, --help                       Display help message
  -v, --version                    Display version information

```

Default configuration file location: ~/.config/torg/config.ini

## Configuration
torg uses INI-format configuration files. 

### Sample Configuration File

```ini

[filter]
; Enable filtering by file type. If disabled, all files are copied.
enabled = true
; Specify file extensions to copy (comma-separated). Required if filter is enabled.
file_types = .wav, .flac

[core]
; Source directory containing files to organize
base_dir = Music
; Destination directory where organized files will be copied
dest_dir = Music2
; Prefix for renamed files (will be followed by sequential numbers)
name_prefix = file_
; Generate a mapping dictionary (true/false) to track old → new file names
generate_dict = true

```

## Sample directory behavior
### Given an input tree like:

```
Music/
  Artist1/
    Song1.wav
    Song2.flac
  Artist2/
    Song3.wav

```

torg will copy matching files under the destination tree while keeping the same hierarchy:

```
Music2/
  Artist1/
    file\_1.wav
    file\_2.flac
  Artist2/
    file\_3.wav

```

If generate_dict = true, a mapping of old names to new names is also produced.


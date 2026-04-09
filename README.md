# Folder-Alias

A command-line tool for managing folder aliases on Windows. Quickly open frequently used directories with short alias names.

## Features

- Create custom aliases for folder paths
- Open folders with simple commands
- List all configured aliases
- Remove unused aliases
- Persistent storage in local configuration file

## Usage

```bash
fa <command> [arguments]
```

### Commands

| Command | Description |
|---------|-------------|
| `version` | Show version information |
| `ls` | List all alias mappings |
| `help` | Show help message |
|`update`|Open github release page |
| `<alias>` | Open folder by alias name |
| `add <alias> <path>` | Add or update an alias mapping |
| `rm <alias>` | Remove an alias mapping |

### Examples

```bash
# Display version
fa version

# Show all aliases
fa ls

# Open github release page
fa update

# Open folder mapped to 'projects'
fa example1

# Create alias 'docs' for D:\Documents
fa add example1 D:\example1

# Remove the 'docs' alias
fa rm example1
```

## Configuration

Alias mappings are stored in `fa_mappings.txt` in the same directory as the executable.

Format:
```
alias=path
```

## Environment Variables

Configure environment variables as needed for your setup.

## Compile command

```bash
g++ -o fa.exe main.cpp -lshell32 -static-libgcc -static-libstdc++
```
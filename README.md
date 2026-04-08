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
| `<alias>` | Open folder by alias name |
| `add <alias> <path>` | Add or update an alias mapping |
| `rm <alias>` | Remove an alias mapping |

### Examples

```bash
# Display version
fa version

# Show all aliases
fa ls

# Open folder mapped to 'projects'
fa projects

# Create alias 'docs' for D:\Documents
fa add docs D:\Documents

# Remove the 'docs' alias
fa rm docs
```

## Configuration

Alias mappings are stored in `fa_mappings.txt` in the same directory as the executable.

Format:
```
alias=path
```

## Environment Variables

Configure environment variables as needed for your setup.

# GitHub API in C

This project is a small C client for the GitHub API. It was created for
recreational and learning purposes, not as a product or a production-ready
tool. It demonstrates HTTP requests with libcurl, JSON processing with cJSON,
and automated tests with CUnit.

## Features

- List public and private repositories accessible to the authenticated user.
- Create a repository.
- Delete a repository.

The `--list_mr` option has not been implemented yet.

## Dependencies

On Debian or Ubuntu, install the build dependencies:

```sh
sudo apt-get update
sudo apt-get install build-essential libcurl4-openssl-dev libcjson-dev libcunit1-dev
```

## Build and test

Run these commands from the project root:

```sh
make          # build build/github
make test     # build and run the tests
make clean    # remove object files
make fclean   # remove object files and generated executables
make re       # clean everything and rebuild
```

## Authentication and usage

The program reads its token from the `GITHUB_TOKEN` environment variable. Set
it in your shell before running a command:

```sh
export GITHUB_TOKEN="your-github-token"
./build/github --list
```

The token must have permission to read the repositories you want to list.
Private repositories are included when the token is authorized to access them.

Available options:

```text
./build/github --help
./build/github --list
./build/github --create REPOSITORY_NAME
./build/github --delete REPOSITORY_NAME
./build/github --list_mr REPOSITORY_NAME  # not implemented yet
```

Keep the token out of the source code and do not share it. API requests verify
TLS certificates.

## HTML documentation

The GitHub Actions workflow builds the project, runs the tests, and performs
static checks. If those steps pass on a push to `main` and the push changes
files under `src/` or the `Doxyfile`, the workflow generates the Doxygen
documentation and updates `doc/html/`. Changes only to the README or other
files do not trigger a documentation update.

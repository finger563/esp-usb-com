# ESP-USB-COM

Example repository for using TinyUSB on an ESP32-S3 to provide a software USB
CDC (communications device class).

Specifically, example is designed to test what, if any, impact large numbers of
CDC / COM ports may have on Windows. The goal is not to have them all at once,
but to see if the additional registry entries may pose an issue when the number
of devices stored gets into the 1000s or 10,000s (or higher). This is because
Windows will assign a new COM port for each device that reports as a unique
serial number.

## Development

This repository is designed to be used as a template repository - so you can
sepcify this as the template repository type when creating a new repository on
GitHub.

After setting this as the template, make sure to update the following:

## Cloning

Since this repo contains a submodule, you need to make sure you clone it
recursively, e.g. with:

``` sh
git clone --recurse-submodules https://github.com/finger563/esp-usb-com
```

Alternatively, you can always ensure the submodules are up to date after cloning
(or if you forgot to clone recursively) by running:

``` sh
git submodule update --init --recursive
```

## Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Output

Example screenshot showing the serial number changing as the device resets:
![CleanShot 2025-03-12 at 16 50 34](https://github.com/user-attachments/assets/1e42a1ad-a32a-49d7-86c5-616e3e43b69a)

## Developing

If you're developing code for this repository, it's recommended to configure
your development environment:

### Code style

1. Ensure `clang-format` is installed
2. Ensure [pre-commit](https://pre-commit.com) is installed
3. Set up `pre-commit` for this repository:

  ``` console
  pre-commit install
  ```

This helps ensure that consistent code formatting is applied, by running
`clang-format` each time you change the code (via a git pre-commit hook) using
the [./.clang-format](./.clang-format) code style configuration file.

If you ever want to re-run the code formatting on all files in the repository,
you can do so:

``` console
pre-commit run --all-files
```

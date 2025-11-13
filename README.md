#### For more info on setting up and running Gecko's frontend Vue application, see the [/frontend README](/frontend)
#### For more info on the CCITT Group 4-based bitonal compression.lib, see the [/shared README](/shared/compression)

# Gecko
WIP full-stack program that lets anyone send an image to my E-Ink display

### Quick Start
Gecko is designed to be both portable and easy to configure. First, make sure `CMake >=3.16` and `npm` are installed on your machine. Then, run:

```bash
cmake -A x64 -B out/build -S . -G "<Your CMake Generator of Choice Here>"
```

> [!NOTE]
> For subsequent configurations, simply run `reconfig` from the command line.

To build all C/C++ projects, run:

```bash
build-all
```

To run the HTTP backend api, run:

```bash
api
```

To run the mosquitto MQTT server, run:

```bash
mosquitto
```

To run the Vite development server, see the [/frontend README](/frontend)
> [!NOTE]
> This repository is a work in progress. Documentation may (will) be incomplete.

# Gecko
Application that allows anyone to send an image to my E-Ink display.

**Development Machine:** Linux / WSL

**Development Dependencies:**
- cmake
- npm
- mysql
- mosquitto
- openssl-dev
- python3
- gcc-arm-none-eabi
- libnewlib-arm-none-eabi
- libstdc++-arm-none-eabi-newlib


### Building Gecko

To configure your environment for development, run:

```sh
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -G *YOUR_GENERATOR_HERE*
```

> [!NOTE]
> Use Ninja or Make. Visual Studio does not work. I'm not sure why Visual Studio doesn't work, but it might have something to do with the fact that it's Visual Studio.

The root CMakelists.txt is a superbuild of all of the other projects. To build a given project, specify it as a build target:

```sh
cmake --build build --target api
```

```sh
cmake --build build --target compression_wasm
```

```sh
cmake --build build --target embedded
```


### Running the backend

Gecko's backend expects a .env file to specify server configuration. Set up your environment like so:

```sh
./scripts/env.sh all 
./scripts/init.sh all
```

Make sure mosquitto and mysql are running:

```sh
./scripts/start.sh mosquitto 
./scripts/start.sh mysql
```

And finally start up the api:

```sh
./scripts/start.sh api
```

### Running the frontend

Gecko's frontend uses Vue as its framework and Vite as its build tool. Start by making sure the compression_wasm target has been built as shown above. Then enter the /frontend/ directory and run `npm install`. You're now ready to run the application:

```sh
npm run dev
```
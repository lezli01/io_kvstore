# Key Value Store

## How to build

### Using Makefile

```
$ make
```

### Using CMake
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Usage

### Server
```
$ server --help
usage: ./server [-h | --help] [port]

Start key value server on configured TCP port on default interface.

  port PORT value to open the TCP socket on. [default: 5555]

```

Simply start the server with the following command:
```
$ build/server
Listening on port: 5555
```

Or on a specific port:
```
$ build/server 1234
Listening on port: 1234
```

### Client
Client is using Python 3.

```
$ src/client/client.py --help
usage: client.py [-h] [-a ADDRESS] [-p PORT] {GET,PUT} ...

Client for key-value storage.

positional arguments:
  {GET,PUT}             command to execute
    GET                 get stored value for a key
    PUT                 put value for a key

optional arguments:
  -h, --help            show this help message and exit
  -a ADDRESS, --address ADDRESS
                        server address to connect to (default: 127.0.0.1)
  -p PORT, --port PORT  server port to connect to (default: 5555)
```

Setting a value:
```
$ src/client/client.py PUT keyToSet valueToSet
Value successfuly stored: keyToSet => valueToSet
```

Getting a value:
```
$ src/client/client.py GET keyToSet           
valueToSet
```

## Limitations
* For each key and value the length can not exceed 2^16.

## Unit Test
To run the tests:
```
$ make run_unit_test
```

Or:
```
$ make unit_test
$ build/unit_test
```

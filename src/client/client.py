#!/usr/bin/env python3

import argparse
import socket
import struct

arg_parser = argparse.ArgumentParser(
    description='Client for key-value storage.',
    formatter_class=argparse.ArgumentDefaultsHelpFormatter
)

arg_parser.add_argument(
    '-a',
    '--address',
    dest='address',
    type=str,
    help='server address to connect to',
    default='127.0.0.1'
)

arg_parser.add_argument(
    '-p',
    '--port',
    dest='port',
    type=int,
    help='server port to connect to',
    default=5555
)

sub_parsers = arg_parser.add_subparsers(
    dest='command',
    help='command to execute'
)

get_command = sub_parsers.add_parser(
    'GET',
    help='get stored value for a key'
)

get_command.add_argument(
    'key',
    type=str,
    help='key to get the value of'
)

put_command = sub_parsers.add_parser(
    'PUT',
    help='put value for a key'
)

put_command.add_argument(
    'key',
    type=str,
    help='key to store the value under'
)

put_command.add_argument(
    'value',
    type=str,
    help='value to store'
)

args = arg_parser.parse_args()

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((args.address, args.port))

if args.command == 'GET':
    client_socket.send('G'.encode())
    client_socket.send(struct.pack('<I', len(args.key)))
    client_socket.send(args.key.encode())

    respLength = struct.unpack('<I', client_socket.recv(4))[0]

    if respLength == 0:
        print('No value found with provided key: ' + args.key)
    else:
        resp = client_socket.recv(respLength).decode()
        print(resp)
elif args.command == 'PUT':
    client_socket.send('P'.encode())
    client_socket.send(struct.pack('<I', len(args.key)))
    client_socket.send(args.key.encode())
    client_socket.send(struct.pack('<I', len(args.value)))
    client_socket.send(args.value.encode())

    resp = client_socket.recv(len(args.value)).decode()

    if resp == args.value:
        print('Value successfuly stored: ' + args.key + " => " + args.value)
    else:
        print('Error occured, value not stored.')

client_socket.close()

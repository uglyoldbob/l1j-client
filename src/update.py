#!/usr/bin/env python
"""Lineage 1.5 Game Update Server
"""

import os
import re
import sys
import gzip
import time
import shutil
import socket
import signal
import struct
import logging
import argparse
import threading
import multiprocessing

from collections import defaultdict
from logging.handlers import WatchedFileHandler

try:
    from Crypto.Cipher import DES
except ImportError:
    print >>sys.stderr, ('pycrypto needs to be installed! You can install it '
                         'via `apt-get install python-crypto`')
    sys.exit(1)

__author__ = 'Tommy Allen'
__email__ = 'tommy@tweek.us'
__copyright__ = 'Copyright 2013, NeverNull LTD'
__license__ = 'RSFB (Ritual Sacrifice of First Born)'
__credits__ = ['Thomas Epperson', 'James Covey', 'Your Mom']
__version__ = '1.0'
__maintainer__ = 'Tommy Allen'


_des = DES.new('~!@#%^$<', DES.MODE_ECB)

LOG_LEVEL = logging.INFO
LOG_FILE = '/var/log/lineage/update-server.log'
PID_FILE = '/tmp/l15update.pid'

UPDATE_DIR = '/home/lineage_web/game-updates/upserv/'
UPDATE_CUE = os.path.join(UPDATE_DIR, '.update')

BINARY_DIR = '/home/lineage/game-updates/'
CACHE_DIR = '/var/tmp/l1updatecache/'

# Half a megabyte seems good
SEND_CHUNK_SIZE = 1024 * 512

SIG_TERM = False
PRINT_STDOUT = True

_update_files = None
_ip_history = defaultdict(int)


def htons(n):
    return struct.pack('!H', n)


def htonl(n):
    return struct.pack('!I', n)


def byte(n):
    return struct.pack('B', n)


def ntohl(data):
    return struct.unpack('!I', data)[0]


def makepath(filename):
    dirname = os.path.dirname(filename)
    if not os.path.exists(dirname):
        umask = os.umask(0)
        os.makedirs(dirname, 0755)
        os.umask(umask)


def get_logger(name):
    logger = logging.getLogger(name)
    format = logging.Formatter('[%(levelname)s] %(asctime)s - %(name)s - %(message)s',
                                datefmt='%m-%d-%Y %H:%M:%S')
    logger.propagate = False

    if PRINT_STDOUT:
        handler = logging.StreamHandler()
        handler.setLevel(LOG_LEVEL)
        handler.setFormatter(format)
        logger.addHandler(handler)

    handler = WatchedFileHandler(LOG_FILE)
    handler.setLevel(LOG_LEVEL)
    handler.setFormatter(format)
    logger.addHandler(handler)
    return logger


def ip_check(ip):
    """Checks an IP to see if it's being rate limited.

    Rate limit is one connection every 2 seconds.

    This is here to prevent an IP from hammering the update server.
    Just in case.
    """
    now = int(time.time())

    if not _ip_history[ip]:
        _ip_history[ip] = now
        return True

    last = _ip_history[ip]
    _ip_history[ip] = now
    if now - last >= 2:
        return True
    return False


def watch_sigterm(interval):
    """A decorator to monitor the global SIG_TERM variable.

    This is meant to be ran as a thread.
    """
    # The loop sleeps for half a second, so the interval should be doubled for
    # the counter.  Not "exact" but close enough.  Sue me.
    interval = interval * 2

    def wrapper(func):
        def inner(*args, **kwargs):
            i = 0
            logger = get_logger('watch:%s' % func.__name__)
            while not SIG_TERM:
                # Sleep for half a second so that SIG_TERM can be caught
                # without a long delay.
                time.sleep(0.5)
                i = i + 1
                if i < interval:
                    continue
                i = 0
                logger.debug('Calling thread function')
                func(*args, **kwargs)
            logger.info('Stopping thread')
        return inner
    return wrapper


@watch_sigterm(60)
def clean_ip_hist():
    """Cleans up the IP history.
    """
    now = int(time.time())
    for ip in _ip_history.keys():
        if now - _ip_history[ip] >= 2:
            del _ip_history[ip]


@watch_sigterm(30)
def child_reaper():
    """Child Reaper

    This is ran in a standard thread and not a separate process.

    Since the socket server waits for new connections, join() can't be called
    after a worker process finishes.  This acts a way to periodicially join()
    the worker processes by calling multiprocessing.active_children() since
    that function cleans up zombie workers before returning a list of active
    workers.
    """
    multiprocessing.active_children()


def load_files():
    global _update_files

    need_update = os.path.exists(UPDATE_CUE)
    if _update_files and not need_update:
        return _update_files

    logger = get_logger('updater')

    if need_update:
        logger.debug('Copying updated files')
        l = len(UPDATE_DIR)
        for root, dirs, files in os.walk(UPDATE_DIR):
            for filename in files:
                src_path = os.path.join(root, filename)
                if src_path == UPDATE_CUE:
                    continue
                bin_name = src_path[l:]
                dst_path = os.path.join(BINARY_DIR, bin_name)
                shutil.copyfile(src_path, dst_path)
                logger.debug('Copying: %s' % bin_name)

        for root, dirs, files in os.walk(UPDATE_DIR):
            for d in dirs:
                shutil.rmtree(os.path.join(root, d))
        os.remove(UPDATE_CUE)

    update_files = []
    bdir_l = len(BINARY_DIR)

    for root, dirs, files in os.walk(BINARY_DIR):
        for filename in files:
            up_file = os.path.join(root, filename)
            name = up_file[bdir_l:]

            gz_file = '%s.gz' % os.path.join(CACHE_DIR, name)

            makepath(gz_file)

            up_mtime = int(os.path.getmtime(up_file))
            gz_mtime = 0
            gz_size = 0
            if os.path.exists(gz_file):
                gz_mtime = int(os.path.getmtime(gz_file))
                gz_size = os.path.getsize(gz_file)

            if up_mtime != gz_mtime:
                logger.debug('Compressing file: %s' % filename)
                gz = gzip.GzipFile(gz_file, mode='wb', mtime=up_mtime)
                with open(up_file, 'rb') as fp:
                    if name.startswith('text/'):
                        # Text file needs to be encrypted with a super secure
                        # key.
                        gz.write('\x04\x03\x02\x01')
                        data = fp.read()
                        data_l = len(data)
                        pad = 8 - (data_l % 8)
                        gz.write(_des.encrypt(data + ('\x00' * pad)))
                    else:
                        # Want to avoid reading large amounts of data into
                        # memory.  It's all GC'd, so just being safe.
                        while True:
                            chunk = fp.read(SEND_CHUNK_SIZE)
                            if chunk:
                                gz.write(chunk)
                            else:
                                break
                gz.close()
                os.utime(gz_file, (up_mtime, up_mtime))
                gz_mtime = up_mtime
                gz_size = os.path.getsize(gz_file)

            update_files.append((up_mtime, gz_size, name))

    # Stored in the global scope as a cache.  Since this is a multiprocess
    # script, I assume the global variables won't be available to the child
    # processes.  So it's cached in the parent and sent to the child when
    # spawning.
    # Sorted with the oldest file first.  The update launcher stores the last
    # timestamp sent with a file in time.dat
    _update_files = tuple(sorted(update_files))
    return _update_files


def send_file(sock, mtime, size, filename):
    """Sends a gzip'd file to the client socket
    """
    gz_filename = '%s.gz' % os.path.join(CACHE_DIR, filename)
    if not os.path.exists(gz_filename):
        return False

    sock.send(byte(len(filename)))
    sock.send(filename)
    sock.send(htonl(size))

    with open(gz_filename, 'rb') as fp:
        try:
            while True:
                buffer = fp.read(SEND_CHUNK_SIZE)
                if buffer:
                    sock.send(buffer)
                else:
                    break
        except:
            # Silently fail and close the file.  Handler will catch the socket
            # error.
            pass

    # Send the modified time.  The updater stores the LAST one it has to
    # compare with future updates.
    sock.send(htonl(mtime))

    return True


def handle_client(connection, address, files):
    """Handle a client connection
    """
    logger = get_logger('worker:%s' % ':'.join([str(x) for x in address]))

    try:
        logger.info('Connected to client')
        try:
            data = connection.recv(4)
            if not data or len(data) < 4:
                logger.error('Bad data from client: %r' % data)
            else:
                logger.debug('Client sent data: %r' % data)
                lastupdate = ntohl(data)

                logger.info('Client last update: %d' % lastupdate)

                # Filter out the unneeded files.
                updates = [x for x in files if x[0] > lastupdate]

                n_updates = len(updates)
                logger.info('Updates to send: %d' % n_updates)
                connection.send(htonl(n_updates))

                if n_updates:
                    for mtime, size, filename in updates:
                        logger.debug('Sending file: %s (%d, %d)' % \
                                     (filename, size, mtime))
                        send_file(connection, mtime, size, filename)

                    # Since files send the length of the filename before a new
                    # file is sent.  I assume that this tells the launcher that
                    # there are no new files.
                    connection.send(htonl(0))

                connection.send(htonl(0x1000000))
                connection.send(htons(0))
        except socket.error, (value, message):
            logger.error('Socket Error %s: %s' % (value, message))
        else:
            if not data:
                logger.info('Connection was closed by remote host')
    except Exception, e:
        logger.error('Unknown error %s' % str(e))
    finally:
        logger.info('Closing client socket')
        connection.close()


def start(address='0.0.0.0', port=2003, backlog=10):
    """Starts the main update server loop.
    """
    global SIG_TERM

    logger = get_logger('server')

    t = threading.Thread(target=child_reaper)
    t.start()

    t = threading.Thread(target=clean_ip_hist)
    t.start()

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind((address, port))
        sock.listen(backlog)

        logger.info('Starting L15 Update server on %s:%s' % (address, port))

        while not SIG_TERM:
            connection, address = sock.accept()
            if ip_check(address[0]):
                files = load_files()
                args = (connection, address, files)
                process = multiprocessing.Process(target=handle_client,
                                                  args=args)
                process.daemon = True
                process.start()
            else:
                # Probably should have a fail2ban rule for this
                logger.error('Reconnecting too fast IP: %s' % address[0])
                connection.close()
    except KeyboardInterrupt:
        logger.info('Keyboard Interrupt')
    except socket.error, (value, message):
        logger.error('Socket Error %s: %s' % (value, message))
    finally:
        logger.info('Stopping server')

    SIG_TERM = True
    for process in multiprocessing.active_children():
        logging.info('Shutting down worker process %d' % process.pid)
        process.terminate()
        process.join()


def signal_handler(signum, frame):
    global SIG_TERM
    if signum == signal.SIGTERM:
        SIG_TERM = True


def main():
    """Parses command line arguments and starts the server.
    """
    global LOG_LEVEL
    global PRINT_STDOUT

    def iptype(s):
        if not re.match(r'\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}', s):
            raise argparse.ArgumentError('%s is not an IP address.' % s)
        return s

    p = argparse.ArgumentParser(description='L15 Update Server')
    p.add_argument('-a', '--address', type=iptype, default='0.0.0.0',
                    help='IP address to bind')
    p.add_argument('-p', '--port', type=int, default=2003,
                    help='Port to listen on')
    p.add_argument('-l', '--loglevel', type=str, default='INFO',
                    choices=('INFO', 'DEBUG'),
                    help='Log level')
    p.add_argument('-d', '--daemon', action='store_true',
                    help='Run server in the background')
    p.add_argument('-q', '--quiet', action='store_true',
                    help='Do not print to stdout')
    args = p.parse_args()

    # If log level isn't found, probably should debug
    LOG_LEVEL = getattr(logging, args.loglevel, 'DEBUG')
    logging.basicConfig(level=LOG_LEVEL)

    PRINT_STDOUT = not args.quiet
    signal.signal(signal.SIGTERM, signal_handler)

    if args.daemon:
        PRINT_STDOUT = False
        pid = os.fork()
        if pid:
            with open(PID_FILE, 'w') as fd:
                fd.write(str(pid))
            get_logger('main').info('Server PID: %d' % pid)
            print 'L15 Update server started with PID %d' % pid
        else:
            start(args.address, args.port)
            if os.path.exists(PID_FILE):
                os.remove(PID_FILE)
    else:
        start(args.address, args.port)


if __name__ == '__main__':
    main()

#!/usr/bin/env python
# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import BaseHTTPServer
import imp
import logging
import multiprocessing
import optparse
import os
import SimpleHTTPServer  # pylint: disable=W0611
import socket
import sys
import time
import urlparse


SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
NACL_SDK_ROOT = os.path.dirname(SCRIPT_DIR)


# We only run from the examples directory so that not too much is exposed
# via this HTTP server.  Everything in the directory is served, so there should
# never be anything potentially sensitive in the serving directory, especially
# if the machine might be a multi-user machine and not all users are trusted.
# We only serve via the loopback interface.
def SanityCheckDirectory(dirname):
  abs_serve_dir = os.path.abspath(dirname)

  # Verify we don't serve anywhere above NACL_SDK_ROOT.
  if abs_serve_dir[:len(NACL_SDK_ROOT)] == NACL_SDK_ROOT:
    return
  logging.error('For security, httpd.py should only be run from within the')
  logging.error('example directory tree.')
  logging.error('Attempting to serve from %s.' % abs_serve_dir)
  logging.error('Run with --no_dir_check to bypass this check.')
  sys.exit(1)


class PluggableHTTPServer(BaseHTTPServer.HTTPServer):
  def __init__(self, *args, **kwargs):
    BaseHTTPServer.HTTPServer.__init__(self, *args)
    self.serve_dir = kwargs.get('serve_dir', '.')
    self.test_mode = kwargs.get('test_mode', False)
    self.delegate_map = {}
    self.running = True
    self.result = 0

  def Shutdown(self, result=0):
    self.running = False
    self.result = result


class PluggableHTTPRequestHandler(SimpleHTTPServer.SimpleHTTPRequestHandler):
  def _FindDelegateAtPath(self, dirname):
    # First check the cache...
    logging.debug('Looking for cached delegate in %s...' % dirname)
    handler_script = os.path.join(dirname, 'handler.py')

    if dirname in self.server.delegate_map:
      result = self.server.delegate_map[dirname]
      if result is None:
        logging.debug('Found None.')
      else:
        logging.debug('Found delegate.')
      return result

    # Don't have one yet, look for one.
    delegate = None
    logging.debug('Testing file %s for existence...' % handler_script)
    if os.path.exists(handler_script):
      logging.debug(
          'File %s exists, looking for HTTPRequestHandlerDelegate.' %
          handler_script)

      module = imp.load_source('handler', handler_script)
      delegate_class = getattr(module, 'HTTPRequestHandlerDelegate', None)
      delegate = delegate_class()
      if not delegate:
        logging.warn(
            'Unable to find symbol HTTPRequestHandlerDelegate in module %s.' %
            handler_script)

    return delegate

  def _FindDelegateForURLRecurse(self, cur_dir, abs_root):
    delegate = self._FindDelegateAtPath(cur_dir)
    if not delegate:
      # Didn't find it, try the parent directory, but stop if this is the server
      # root.
      if cur_dir != abs_root:
        parent_dir = os.path.dirname(cur_dir)
        delegate = self._FindDelegateForURLRecurse(parent_dir, abs_root)

    logging.debug('Adding delegate to cache for %s.' % cur_dir)
    self.server.delegate_map[cur_dir] = delegate
    return delegate

  def _FindDelegateForURL(self, url_path):
    path = self.translate_path(url_path)
    if os.path.isdir(path):
      dirname = path
    else:
      dirname = os.path.dirname(path)

    abs_serve_dir = os.path.abspath(self.server.serve_dir)
    delegate = self._FindDelegateForURLRecurse(dirname, abs_serve_dir)
    if not delegate:
      logging.info('No handler found for path %s. Using default.' % url_path)
    return delegate

  def _SendNothingAndDie(self, result=0):
    self.send_response(200, 'OK')
    self.send_header('Content-type', 'text/html')
    self.send_header('Content-length', '0')
    self.end_headers()
    self.server.Shutdown(result)

  def send_head(self):
    delegate = self._FindDelegateForURL(self.path)
    if delegate:
      return delegate.send_head(self)
    return self.base_send_head()

  def base_send_head(self):
    return SimpleHTTPServer.SimpleHTTPRequestHandler.send_head(self)

  def do_GET(self):
    # TODO(binji): pyauto tests use the ?quit=1 method to kill the server.
    # Remove this when we kill the pyauto tests.
    _, _, _, query, _ = urlparse.urlsplit(self.path)
    if query:
      params = urlparse.parse_qs(query)
      if '1' in params.get('quit', None):
        self._SendNothingAndDie()
        return

    delegate = self._FindDelegateForURL(self.path)
    if delegate:
      return delegate.do_GET(self)
    return self.base_do_GET()

  def base_do_GET(self):
    return SimpleHTTPServer.SimpleHTTPRequestHandler.do_GET(self)

  def do_POST(self):
    delegate = self._FindDelegateForURL(self.path)
    if delegate:
      return delegate.do_POST(self)
    return self.base_do_POST()

  def base_do_POST(self):
    if self.server.test_mode:
      if self.path == '/ok':
        self._SendNothingAndDie(0)
      elif self.path == '/fail':
        self._SendNothingAndDie(1)


class LocalHTTPServer(object):
  """Class to start a local HTTP server as a child process."""

  def __init__(self, dirname, port, test_mode):
    parent_conn, child_conn = multiprocessing.Pipe()
    self.process = multiprocessing.Process(
        target=_HTTPServerProcess,
        args=(child_conn, dirname, port, {
          'serve_dir': dirname,
          'test_mode': test_mode,
        }))
    self.process.start()
    if parent_conn.poll(10):  # wait 10 seconds
      self.port = parent_conn.recv()
    else:
      raise Exception('Unable to launch HTTP server.')

    self.conn = parent_conn

  def ServeForever(self):
    """Serve until the child HTTP process tells us to stop.

    Returns:
      The result from the child (as an errorcode), or 0 if the server was
      killed not by the child (by KeyboardInterrupt for example).
    """
    child_result = 0
    try:
      # Block on this pipe, waiting for a response from the child process.
      child_result = self.conn.recv()
    except KeyboardInterrupt:
      pass
    finally:
      self.Shutdown()
    return child_result

  def ServeUntilSubprocessDies(self, process):
    """Serve until the child HTTP process tells us to stop or |subprocess| dies.

    Returns:
      The result from the child (as an errorcode), or 0 if |subprocess| died,
      or the server was killed some other way (by KeyboardInterrupt for
      example).
    """
    child_result = 0
    try:
      while True:
        if process.poll() is not None:
          child_result = 0
          break
        if self.conn.poll():
          child_result = self.conn.recv()
          break
        time.sleep(0)
    except KeyboardInterrupt:
      pass
    finally:
      self.Shutdown()
    return child_result

  def Shutdown(self):
    """Send a message to the child HTTP server process and wait for it to
        finish."""
    self.conn.send(False)
    self.process.join()

  def GetURL(self, rel_url):
    """Get the full url for a file on the local HTTP server.

    Args:
      rel_url: A URL fragment to convert to a full URL. For example,
          GetURL('foobar.baz') -> 'http://localhost:1234/foobar.baz'
    """
    return 'http://localhost:%d/%s' % (self.port, rel_url)


def _HTTPServerProcess(conn, dirname, port, server_kwargs):
  """Run a local httpserver with the given port or an ephemeral port.

  This function assumes it is run as a child process using multiprocessing.

  Args:
    conn: A connection to the parent process. The child process sends
        the local port, and waits for a message from the parent to
        stop serving. It also sends a "result" back to the parent -- this can
        be used to allow a client-side test to notify the server of results.
    dirname: The directory to serve. All files are accessible through
       http://localhost:<port>/path/to/filename.
    port: The port to serve on. If 0, an ephemeral port will be chosen.
    server_kwargs: A dict that will be passed as kwargs to the server.
  """
  try:
    os.chdir(dirname)
    httpd = PluggableHTTPServer(('', port), PluggableHTTPRequestHandler,
                                **server_kwargs)
  except socket.error as e:
    sys.stderr.write('Error creating HTTPServer: %s\n' % e)
    sys.exit(1)

  try:
    conn.send(httpd.server_address[1])  # the chosen port number
    httpd.timeout = 0.5  # seconds
    while httpd.running:
      # Flush output for MSVS Add-In.
      sys.stdout.flush()
      sys.stderr.flush()
      httpd.handle_request()
      if conn.poll():
        httpd.running = conn.recv()
  except KeyboardInterrupt:
    pass
  finally:
    conn.send(httpd.result)
    conn.close()


def main(args):
  parser = optparse.OptionParser()
  parser.add_option('-C', '--serve-dir',
      help='Serve files out of this directory.',
      dest='serve_dir', default=os.path.abspath('.'))
  parser.add_option('-p', '--port',
      help='Run server on this port.',
      dest='port', default=5103)
  parser.add_option('--no_dir_check',
      help='No check to ensure serving from safe directory.',
      dest='do_safe_check', action='store_false', default=True)
  parser.add_option('--test-mode',
      help='Listen for posts to /ok or /fail and shut down the server with '
          ' errorcodes 0 and 1 respectively.',
      dest='test_mode', action='store_true')
  options, args = parser.parse_args(args)
  if options.do_safe_check:
    SanityCheckDirectory(options.serve_dir)

  server = LocalHTTPServer(options.serve_dir, int(options.port),
                           options.test_mode)

  # Serve until the client tells us to stop. When it does, it will give us an
  # errorcode.
  print 'Serving %s on %s...' % (options.serve_dir, server.GetURL(''))
  return server.ServeForever()

if __name__ == '__main__':
  sys.exit(main(sys.argv[1:]))

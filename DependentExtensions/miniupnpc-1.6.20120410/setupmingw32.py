#! /usr/bin/python
# $Id: setupmingw32.py,v 1.7 2012/03/05 04:54:00 nanard Exp $
# the MiniUPnP Project (c) 2007-2012 Thomas Bernard
# http://miniupnp.tuxfamily.org/ or http://miniupnp.free.fr/
#
# python script to build the miniupnpc module under windows (using mingw32)
#
from distutils.core import setup, Extension
from distutils import sysconfig
sysconfig.get_config_vars()["OPT"] = ''
sysconfig.get_config_vars()["CFLAGS"] = ''
setup(name="miniupnpc", version="1.6",
      ext_modules=[
	         Extension(name="miniupnpc", sources=["miniupnpcmodule.c"],
	                   libraries=["ws2_32", "iphlpapi"],
			           extra_objects=["libminiupnpc.a"])
			 ])


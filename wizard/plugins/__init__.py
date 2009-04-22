#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2009 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import os
import glob

_modules = glob.glob(os.path.dirname(os.path.abspath(__file__)) + os.sep + "*.py")
_modules = [os.path.basename(module) for module in _modules]
__all__ = []
for module in _modules:
    if module != "__init__.py":
        __all__.append(module.replace(".py", ""))

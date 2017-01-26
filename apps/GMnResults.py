#!/usr/bin/env python

# This line is causing issues: /usr/share/vim/vim74/ftplugin/python.vim
# setlocal expandtab shiftwidth=4 softtabstop=4 tabstop=8
# which I'll change to
# setlocal expandtab shiftwidth=2 softtabstop=2 tabstop=2
#import sys ## Interactions with some system features
#import os ## Access os features (like modifying filesystem)
#import stat ## For status/attributes of a file
#import re # For regular expressions
#import subprocess ## So we can run commands
#import tempfile ## For temporary file/directory creation

#from jinja2 import Environment, FileSystemLoader
from GMnDB import *

db = GMnDB()

## Main Function (program starts here)
def main():
  ## Load up the results database
  db.loadDB()
  #db.printDebug()

## Run main() function if this file is run directly in the command line
## (as opposed to being imported)
if __name__ == '__main__':
  main()

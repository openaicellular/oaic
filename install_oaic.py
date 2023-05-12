#!/usr/bin/env python3
import subprocess
subprocess.run("apt install -y  python3-pip", shell=True, check=True)
subprocess.check_call([ 'pip3', 'install','docutils'])
subprocess.check_call([ 'pip3', 'install','Pygments'])

import docutils.nodes
import docutils.parsers.rst
import docutils.utils
import docutils.frontend
from docutils.core import publish_doctree
import sys
import os
from io import StringIO


subprocess.run("git clone https://github.com/openaicellular/oaic.git",shell=True)
os.chdir("oaic")
subprocess.run("git submodule update --init --recursive --remote",shell=True)


fileNames= ['docs/source/installation.rst']
rstTexts= [] 

for file in fileNames:
  with open(file,'r') as f:
      rstText= f.read()

  rstTexts.append(rstText)

def is_bash_code_block(node):
    return (node.tagname == 'literal_block'
            and 'code' in node.attributes['classes'] and 'bash' in node.attributes['classes'])

source_code=[]
try:
  for rstText in rstTexts:
    doctree = publish_doctree(rstText)
    code_blocks = doctree.traverse(condition=is_bash_code_block)
    source_code.extend( [block.astext() for block in code_blocks])
except:
  print("An exception occured")

with open ('run.sh', 'w') as rsh:
    rsh.write('\n'.join(source_code))
   

subprocess.run("sudo bash run.sh", shell=True, check=True)


#!/usr/bin/python
# -*- coding: utf-8 -*-

## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##      http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.

import pynini
import sys


def Tester(stream, far_reader):
  """Tests rules against inputs, checking outputs.

  Args:
    stream:     input stream
    far_reader: a far reader object

  Returns: None
  """
  lineno = -1
  success = True
  for line in stream:
    lineno += 1
    fields = line.strip('\n').split('\t')
    if len(fields) != 3:
      sys.stderr.write('Skipping line %d (wrong number of fields)\n' % lineno)
      continue
    rules, input_, output = fields
    input_fst = input_
    rule_failure = False
    for rule in rules.split(','):
      try:
        rule_fst = far_reader[rule]
      except KeyError:
        sys.stderr.write('Warning: cannot find rule %s, line %d\n'
                         % (rule, lineno))
        rule_failure = True
        break
      input_fst = input_fst * rule_fst
    if rule_failure:
      success = False
      continue
    ofst = pynini.shortestpath(pynini.project(input_fst, True))
    it = pynini.StringPaths(ofst)
    pred = ''
    while not it.done():
      ## Accepts the first string
      pred = it.istring()
      break
    if pred != output:
      success = False
      sys.stderr.write('Line %d: input and output do not match for\n'
                       '   Rules:\t%s\n'
                       '   Input:\t%s\nExpected:\t%s\n  Actual:\t%s\n' %
                       (lineno, rules, input_, output, pred))
  if success:
    sys.stderr.write('All tests pass!!\n')
  else:
    sys.stderr.write('Some rewrites failed\n')


if __name__ == '__main__':
  USAGE_  = '%s farfile testfile'
  if len(sys.argv) != 3:
    sys.stderr.write('Usage: %s\n' % (USAGE_ % sys.argv[0]))
    sys.exit(0)
  far_reader = pynini.Far(sys.argv[1], 'r')
  with open(sys.argv[2]) as stream:
    Tester(stream, far_reader)
    
  

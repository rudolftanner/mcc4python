"""
SimpleConfigParser

Simple configuration file parser: Python module to parse configuration files
without sections. Based on ConfigParser from the standard library.

Author: Philippe Lagadec

Project website: http://www.decalage.info/python/configparser

Inspired from an idea posted by Fredrik Lundh:
http://mail.python.org/pipermail/python-dev/2002-November/029987.html

Usage: see end of source code and http://docs.python.org/library/configparser.html
"""

__author__ = 'Philippe Lagadec'
__version__ = '0.02'

#--- LICENSE ------------------------------------------------------------------

# The SimpleConfigParser Python module is copyright (c) Philippe Lagadec 2009-2010
#
# By obtaining, using, and/or copying this software and/or its associated
# documentation, you agree that you have read, understood, and will comply with
# the following terms and conditions:
#
# Permission to use, copy, modify, and distribute this software and its
# associated documentation for any purpose and without fee is hereby granted,
# provided that the above copyright notice appears in all copies, and that both
# that copyright notice and this permission notice appear in supporting
# documentation, and that the name of the author not be used in advertising or
# publicity pertaining to distribution of the software without specific,
# written prior permission.
#
# THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
# ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
# THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
# ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#-------------------------------------------------------------------------------
# CHANGELOG:
# 2009-02-12 v0.01 PL: - initial version
# 2010-03-15 v0.02 PL: - updated tests and comments

#-------------------------------------------------------------------------------
# TODO:
# - implement read() using the base class code

#=== IMPORTS ==================================================================

import ConfigParser, StringIO

#=== CONSTANTS ================================================================

# section name for options without section:
NOSECTION = 'NOSECTION'

#=== CLASSES ==================================================================

class SimpleConfigParser(ConfigParser.SafeConfigParser):
    """
    Simple configuration file parser: based on ConfigParser from the standard
    library, slightly modified to parse configuration files without sections.

    Inspired from an idea posted by Fredrik Lundh:
    http://mail.python.org/pipermail/python-dev/2002-November/029987.html
    """

    def __init__(self):
        ConfigParser.SafeConfigParser.__init__(self)
        self.optionxform = str
        self.filename    = None

    def read(self, filename):
        import os

        self.filename = os.path.abspath(filename)

        fileContent = open(self.filename).read()
        modifiedContent = StringIO.StringIO("[NOSECTION]\n" + fileContent)
        self.readfp(modifiedContent)

    def get_filename(self):
        return self.filename

    def section_get(self, section, option, default = None):
        'get the value of an option in the given section'
        if (self.section_has_option(section, option)):
            return self.get(section, option)
        elif default != None:
            return default
        else:
            raise NameError('Cannot find option "' + option + '" in configuration file')

    def outer_get(self, option, default = None):
        'get the value of an option'
        return self.section_get(NOSECTION, option, default)

    def section_evalequation(self, section, option, default = None):
        'get the value of a an equation in the give section'

        import parser

        if (self.section_has_option(section, option)):
            formula = self.get(section, option)
        elif default != None:
            formula = default
        else:
            raise NameError('Cannot find option "' + option + '" in configuration file')

        code = parser.expr(formula).compile()

        return eval(code)

    def outer_evalequation(self, option, default = None):
        'get the value of a an equation'
        return self.section_evalequation(NOSECTION, option, default)

    def section_getboolean(self, section, option, default = None):
        'get the value of a boolean option in the give section'
        if (self.section_has_option(section, option)):
            return self.getboolean(section, option)
        elif default != None:
            return default
        else:
            raise NameError('Cannot find option "' + option + '" in configuration file')

    def outer_getboolean(self, option, default = None):
        'get the value of a boolean option'
        return self.section_getboolean(NOSECTION, option, default)

    def section_getfloat(self, section, option, default = None):
        'get the value of a float option'
        if (self.section_has_option(section, option)):
            return self.getfloat(section, option)
        elif default != None:
            return default
        else:
            raise NameError('Cannot find option "' + option + '" in configuration file')

    def outer_getfloat(self, option, default = None):
        'get the value of a float option'
        return self.section_getfloat(NOSECTION, option, default)

    def section_getint(self, section, option, default = None):
        'get the value of a integer option'
        if (self.section_has_option(section, option)):
            return self.getint(section, option)
        elif default != None:
            return default
        else:
            raise NameError('Cannot find option "' + option + '" in configuration file')

    def outer_getint(self, option, default = None):
        'get the value of a int option'
        return self.section_getint(NOSECTION, option, default)

    def section_options(self, section):
        'get a list of available options in the given section'
        return self.options(section)

    def outer_options(self):
        'get a list of available options'
        return self.section_options(NOSECTION)

    def section_set(self, section, option, value):
        'set the value of an option a section'
        import fileinput
        import sys

        # The "set" method will raise an exception if the requested section
        # does not exist.
        self.set(section, option, value)
        if not self.filename:
            return

        currentSection  = NOSECTION
        previousSection = None
        currentOption   = None
        optionSet       = False
        for rawLine in fileinput.input(self.filename, inplace = 1):
            # If option was set just print the raw
            if optionSet:
                sys.stdout.write(rawLine)
                continue

            # Skip empty line or line starting with # or ;
            line = rawLine.strip()
            if len(line) == 0 or line.startswith('#') or line.startswith(';'):
                sys.stdout.write(rawLine)
                continue

            # Strip comment
            line = line.partition(';')[0].strip()

            # Section
            #
            # First check if line defines a section, then check if the
            # current section is the requested section
            if line.startswith('[') and line.endswith(']'):
                previousSection = currentSection
                currentSection  = line[1:-1]

                # If the requested section was the previous one and the
                # options was not found, add it at the end of the previous
                # section
                if previousSection == section and not optionSet:
                    rawLine = option + " = " + value + "\n" + rawLine
                    sys.stdout.write(rawLine)
                    optionSet = True
                    continue

            if currentSection != section:
                sys.stdout.write(rawLine)
                continue

            # Option
            #
            # First check if line defines a option/value pair, then check if
            # the current option is the requested option. If we have find
            # the option we are looking for, update its value.
            if not "=" in line:
                sys.stdout.write(rawLine)
                continue

            currentOption = line.partition('=')[0].strip()

            if currentOption != option:
                sys.stdout.write(rawLine)
                continue

            comment = rawLine.partition(';')[2].strip()
            rawLine = line.partition('=')[0] + "= " + value
            if comment:
                rawLine += " ; " + comment
            rawLine += "\n"

            sys.stdout.write(rawLine)
            optionSet = True

    def outer_set(self, option, value):
        'set the value of an option'
        return self.section_set(NOSECTION, option, value)

    def section_has_option(self, section, option):
        """
        return True if an option is available in the give section, False otherwise.
        """
        return self.has_option(section, option)

    def outer_has_option(self, option):
        """
        return True if an option is available, False otherwise.
        """
        return self.section_has_option(NOSECTION, option)


#=== MAIN =====================================================================

if __name__ == '__main__':
    # simple tests when launched as a script instead of imported as module:

    ##cp = ConfigParser.ConfigParser()
    ### this raises an exception:
    ### ConfigParser.MissingSectionHeaderError: File contains no section headers.
    ##cp.read('config_without_section.ini')

    print 'SimpleConfigParser tests:'

    filename = 'sample_config_no_section.ini'
    cp = SimpleConfigParser()
    print 'Parsing %s...' % filename
    cp.read(filename)

    print 'Sections:', cp.sections()
    #print cp.items(NOSECTION)
    print 'outer_options():', cp.outer_options()
    for option in cp.outer_options():
        print "outer_get('%s') = '%s'" % (option, cp.outer_get(option))
    print "hasoption('wrongname') =", cp.outer_has_option('wrongname')
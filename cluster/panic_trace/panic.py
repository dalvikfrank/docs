#!/usr/bin/python
# stack symbol parser

import os
import string
import sys

class ReadLog:
    def __init__(self,filename):
        self.logname = filename
    def parse(self):
        f = file(self.logname,'r')
        lines = f.readlines()
        if lines != []:
            print 'read file ok'
        else:
            print 'read file failed'
        result =[]
        for line in lines:
            if line.find('stack') != -1:
                print 'stop search'
                break
            elif line.find('system') != -1:
                #print 'find one item' + line
                result.append(line)
        return result

class ParseContent:
    def __init__(self,addr,lib):
            self.address = addr # pc address
            self.exename = lib  # executable or shared library
    def addr2line(self):
        cmd = addr2line_tool + " -C -f -s -e " + symbol_dir + self.exename + " " + self.address
        #print cmd
        stream = os.popen(cmd)
        lines = stream.readlines();
        list = map(string.strip,lines)
        return list

def printHelp(prog):
    print 'Useage: %s <-w work_path> [-p product_name] <-f file_name>' % (prog)
    print '        %s -h' % (prog)
    print '    -w work_path      the android source code path'
    print '    -p product_name   the android product name'
    print '    -f file_name      the error info name'
    print '    -h                print help info'

hasError = False

def noArgError(option):
    global hasError
    print 'option \'%s\' need argument!' % (option)
    hasError = True

def argError(index, info):
    global hasError
    if index == '-w':
        print 'argument work_path \'%s\' error!' % (info)
    if index == '-p':
        print 'argument product_name \'%s\' error!' % (info)
    if index == '-f':
        print 'argument file_name \'%s\' error!' % (info)
    hasError = True

argv = sys.argv
argc = len(argv)
if argc < 2:
    printHelp(argv[0])
    exit()

#define android product name
ANDROID_PRODUCT_NAME = 'generic'
ANDROID_WORKSPACE = "None"
FILE_NAME = 'None'

arg_index = 1
while arg_index < argc:
#    print 'check argv[%d] (%s) ' % (arg_index, argv[arg_index])
    if argv[arg_index] == '-h':
        printHelp(argv[0])
        exit()
    elif argv[arg_index] == '-w':
        arg_index = arg_index + 1
        if (arg_index == argc):
            noArgError(argv[arg_index - 1])
            continue
        ANDROID_WORKSPACE = argv[arg_index]
    elif argv[arg_index] == '-p':
        arg_index = arg_index + 1
        if (arg_index == argc):
            noArgError(argv[arg_index - 1])
            continue
        ANDROID_PRODUCT_NAME = argv[arg_index]
    elif argv[arg_index] == '-f':
        arg_index = arg_index + 1
        if (arg_index == argc):
            noArgError(argv[arg_index - 1])
            continue
        FILE_NAME = argv[arg_index]
    else :
        print 'error argument ', argv[arg_index]
    arg_index = arg_index + 1

product_dir = ANDROID_WORKSPACE + '/out/target/product/' + ANDROID_PRODUCT_NAME

if ANDROID_WORKSPACE == 'None' or os.path.exists(ANDROID_WORKSPACE) == False:
    argError('-w', ANDROID_WORKSPACE)

if ANDROID_PRODUCT_NAME == 'None' or os.path.exists(product_dir) == False:
    argError('-p', ANDROID_PRODUCT_NAME)
    print 'product_dir is \'%s\'.' % (product_dir)

if FILE_NAME == 'None' or os.path.isfile(FILE_NAME) == False:
    argError('-f', FILE_NAME)

if hasError == True:
    printHelp(argv[0])
    exit()

# addr2line tool path and symbol path
addr2line_tool = ANDROID_WORKSPACE + '/prebuilt/linux-x86/toolchain/arm-eabi-4.2.1/bin/arm-eabi-addr2line'
symbol_dir = product_dir +'/symbols'
symbol_bin = symbol_dir + '/system/bin/'
symbol_lib = symbol_dir + '/system/lib/'

readlog = ReadLog(FILE_NAME)
inputlist = readlog.parse()

for item in inputlist:
    itemsplit = item.split()
    test = ParseContent(itemsplit[-2],itemsplit[-1])
    list = test.addr2line()
    print "%-30s%s" % (list[1],list[0])


#!/usr/bin/python
import urllib2
import re
import os
import sys
import commands
import subprocess
import shlex
import string
import getopt

#PATH='/media/frank/jam/linaro/LMG/LMG-914_static_link'
PATH='/media/frank/jam/linaro/aosp-M'
LIBS_SET='/media/frank/jam/linaro/LMG/LMG-914_static_link/set.txt'
#lib='libmincrypt'
DELTTE_TEST_CASE = True  # True: allCount all count except test cases  Flase: allCount including test cases

def search(path):
    if len(path)==0 :
        print ("Invalid path null !")
    else : 
        #os.environ['sh_path']=str(path)
        #os.system('echo $sh_path')
        #res, output = commands.getstatusoutput('find $sh_path -iname *.mk')
        #print ("res=%s output=%s" %(res, output))
        child = subprocess.Popen(["find", PATH,"-iname","*.mk"], stdout=subprocess.PIPE)
        out = child.communicate() # return out array
        print out
        libs_h = open(LIBS_SET, 'r')
        done = 0
        while not done:
            aLine = libs_h.readline()
            if(aLine != ''):
                lib = aLine.strip()
                print "\n\n\n >>>>>>>>>>>>>>>>>>>>>>>> lib = %s" %(lib)
                mdl = passer_module(out, lib)
                (stc_all, stc_test) = passer_staticlink(out, lib)
                print (stc_all, stc_test, mdl)
                if (stc_all>1):
                    create_file = "/media/frank/jam/linaro/LMG/LMG-914_static_link/output/%dstatic_%dtest_%dmodule_<%s>" %(stc_all, stc_test, mdl, lib)
                    print "Create File: %s" %(create_file)
                    handle = open(create_file, 'wa+')
                    parser(out, lib, create_file, handle)
                    handle.close()
                else:
                    pass
            else:
                done = 1
        libs_h.close()
    return

def search_opt(android_dir, set_file, out_dir):
    if len(android_dir)==0 :
        print ("Invalid path null !")
    else : 
        #os.environ['sh_path']=str(path)
        #os.system('echo $sh_path')
        #res, output = commands.getstatusoutput('find $sh_path -iname *.mk')
        #print ("res=%s output=%s" %(res, output))
        child = subprocess.Popen(["find", PATH,"-iname","*.mk"], stdout=subprocess.PIPE)
        out = child.communicate() # return out array
        print out
        libs_h = open(set_file, 'r')
        done = 0
        while not done:
            aLine = libs_h.readline()
            if(aLine != ''):
                lib = aLine.strip()
                print "\n\n\n >>>>>>>>>>>>>>>>>>>>>>>> lib = %s" %(lib)
                mdl = passer_module(out, lib)
                (stc_all, stc_test) = passer_staticlink(out, lib)
                print (stc_all, stc_test, mdl)
                if (stc_all>1):
                    create_file = "%s/%dstatic_%dtest_%dmodule_<%s>" %(out_dir, stc_all, stc_test, mdl, lib)
                    print "Create File: %s" %(create_file)
                    handle = open(create_file, 'wa+')
                    parser(out, lib, create_file, handle)
                    handle.close()
                else:
                    pass
            else:
                done = 1
        libs_h.close()
    return

def passer_module(out, lib):
    moduleCount = 0
    for f in out[0].split('\n'):
        if len(f)==0:
            continue
        content = readContent(f)
        regexSModule= "(LOCAL_MODULE)\s?(\+\=|\:\=)([^=]*?%s)[^a-zA-Z0-9+]" %(lib)
        patternModule = re.compile(regexSModule, re.S)
        for i in patternModule.findall(content):
            moduleCount = moduleCount + 1
            str = ""
            for j in i:
                str = "%s%s" %(str, j)
    return moduleCount

def passer_staticlink(out, lib):
    allCount = 0
    testCount = 0
    for f in out[0].split('\n'):
        if len(f)==0:
            continue
        content = readContent(f)
        regexStatic = "(LOCAL_STATIC_LIBRARIES|static_libraries)\s?(\+\=|\:\=)([^=]*?%s)[^a-zA-Z0-9+]" %(lib)
        patternStatic = re.compile(regexStatic, re.S)
        for i in patternStatic.findall(content):
            isTest = checkTestCase(f)
            if isTest:
                testCount = testCount + 1
            if (DELTTE_TEST_CASE and isTest):
                continue
            allCount = allCount + 1
    #print "allCount= %d, testCount = %d" %(allCount, testCount)
    return (allCount, testCount)

def parser(out, lib, fout, handle):
    index = 0
    for f in out[0].split('\n'):
        if len(f)==0:
            return
        else:
            content = readContent(f)
            regex="(LOCAL_STATIC_LIBRARIES|LOCAL_MODULE|static_libraries)\s?(\+\=|\:\=)([^=]*?%s)[^a-zA-Z0-9+]" %(lib)
            pattern = re.compile(regex, re.S)
            pre_file = ""

            for i in pattern.findall(content):
                str = ""
                align = ""
                if (f != pre_file):
                    index = index + 1
                    align = "\n\n\n\n%d)  ====%s====\n" %(index, f)
                    pre_file = f
                else:
                    align = "\n" 
                for j in i:
                   str = "%s%s" %(str, j)
                in_file = "%s\n%s" %(align, str)
                print in_file
                handle.write(in_file)
    return

def parser_output():
    pass

def readContent(f):
    handle = file(f, 'rb')
    content = handle.read().strip()
    handle.close()
    return content

def parsertest(f):
    if len(f)==0:
        return
    else:
        content = readContent(f)
        regex="(LOCAL_STATIC_LIBRARIES|LOCAL_MODULE|static_libraries)\s?(\+\=|\:\=)([^=]*?%s)[^a-zA-Z0-9+]" %(lib)
        pattern = re.compile(regex, re.S)
        #pattern = re.compile(r'(LOCAL_STATIC_LIBRARIES|LOCAL_MODULE)\s?(\+\=|\:\=)([^=]*?libc)[^a-zA-Z0-9+]', re.S)
        #print ("%s \n\t %s" %(f, pattern.findall(content)))
        #if match:
        #print ("%s \n\t %s" %(f, match.group()))
        for i in pattern.findall(content):
            str = ""
            for j in i:
               str = "%s%s" %(str, j)
    return

def checkTestCase(f):
    isTest = False
    print "============ %s" %(f)
    for teststr in re.split('/', f):
        #print "spilt to :%s" %(teststr)
        if (teststr == 'test') or (teststr == 'tests') :
            isTest=True
            break
        else:
            continue
    return isTest

def usage():
    print "usage: regrex.py -s SET_FILE -d OUT_DIR"
    print ""
    print "   param: ANDROID_DIR:   androir root directory"
    print "          OUT_DIR:       output the result directory"
    print "                         generate files like:  %d(staticlink)_%d(test case)_%d(module count)_<lib module name>"

def  main():
    #./regrex.py -r /media/frank/jam/linaro/aosp-M -s /media/frank/jam/linaro/LMG/LMG-914_static_link/set.txt -d /media/frank/jam/linaro/LMG/LMG-914_static_link/output
    opts, args = getopt.getopt(sys.argv[1:], "hr:s:d:")
    for op, value in opts:
        if op == "-r":
            android_dir = value
        elif op == "-s":
            set_file = value
        elif op == "-d":
            out_dir = value
        elif op == "-h":
            usage()
            sys.exit()
    search_opt(android_dir, set_file, out_dir)
    #search(PATH)

if __name__ == "__main__":
    main()



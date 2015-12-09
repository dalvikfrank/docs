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

START_STRING="LOCAL_MODULE"
END_STRING='STATIC_LIBRARY'

def search_opt(android_dir, match_file, out_file, set_file):
    if len(android_dir)==0 :
        print ("Invalid path null !")
    else : 
        child = subprocess.Popen(["find", android_dir,"-iname","*.mk"], stdout=subprocess.PIPE)
        out = child.communicate() # return out array

        clear(match_file)

        #parser 1th time, parser all the matches wirte to "match_file"
        for f in out[0].split('\n'):
            print ("prepare parser file=%s" %(f))
            parser(f, match_file)

        #parser 2th time, parser "match_file" and write all the libraries to "out_file"
        parserLibs(match_file, out_file)

        #parser 3th time, the following choose one of 3, parser "out_file" and keep only once each library
        sortAndGrepOnce(out_file, set_file)
        #grepOnce(out_file, set_file)
        #grepOnceWithCount(out_file, set_file)

        #paser 4th: replace all the "+" with "\+" for regular expression
        replace(set_file)
    return

def parser(f, match_file):
    if len(f)==0:
        return
    else:
        content = readContent(f)
        regex="(%s\s\:\=)(((?!LOCAL_MODULE).)*)(%s)" %(START_STRING, END_STRING)
        pattern = re.compile(regex, re.S)
        #print ("%s \n\t %s" %(f, pattern.findall(content)))
        for i in pattern.findall(content):
            str = ""
            for j in i:
                str = "%s%s" %(str, j)
            match = "%s \n %s\n\n\n\n\n" %(f, str)
            print match
            writeFile(match_file, match)
    return

def readContent(f):
    handle = file(f, 'rb')
    content = handle.read().strip()
    handle.close()
    return content

def writeFile(f, str):
    newFile = open(f, 'a')
    newFile.write(str)
    newFile.close()
    return

def clear(f):
    newFile = open(f, 'wb')
    newFile.truncate()
    newFile.close()
    return
def replace(f):
    fp=open(f,'r')  
    alllines=fp.readlines()  
    fp.close()  
    fp=open(f,'w')  
    for eachline in alllines:  
        a=re.sub('\+\+','\\\+\\\+',eachline)  
        fp.writelines(a)  
    fp.close() 

def parserLibs(inf, outf):
    clear(outf)
    f = open(inf, 'r')
    done = 0
    module1 = 'LOCAL_MODULE :='
    module2 = 'LOCAL_MODULE:='

    while not done:
        aLine = f.readline()
        if(aLine != ''):
            magic = aLine.find(module1) + aLine.find(module2)
            if magic > -1:
                aLine = aLine.strip()
                #print "======== found here aLine=%s" %(aLine)
                middle = re.split('[\s]', aLine) #middle[-1] = libxxx
                instr = "%s\n" %(middle[-1])
                #handle "LOCAL_MODULE := \
                if instr =="\\\n":
                    instr = "%s\n" %f.readline().strip()
                writeFile(outf, instr)
            else:
                pass
        else:
            done = 1
    f.close()

def grepOnce(fin, fout):
    lines_seen = set()
    fh = open(fin, 'r')
    oh = open(fout, 'w')
    done = 0
    while not done:
        aLine = fh.readline()
        if(aLine != ''):
            if aLine not in lines_seen:
                oh.write(aLine)
                lines_seen.add(aLine)
            pass
        else:
            done = 1
    fh.close()
    oh.close()
    return

def sortAndGrepOnce(fin, fout):
    lines_seen = list()
    fh = open(fin, 'r')
    oh = open(fout, 'w')
    done = 0
    while not done:
        aLine = fh.readline()
        if(aLine != ''):
            if aLine not in lines_seen:
                lines_seen.append(aLine)
            pass
        else:
            done = 1
    lines_seen.sort()
    for item in lines_seen:
        oh.write(item)
    fh.close()
    oh.close()
    return

def grepOnceWithCount(fin, fout):
    lines_seen = list()
    fh = open(fin, 'r')
    oh = open(fout, 'w')
    array = [0 for x in range(0, 2000)] 
    done = 0
    while not done:
        aLine = fh.readline()
        if(aLine != ''):
            if aLine not in lines_seen:
                lines_seen.append(aLine)
            else:
                pass
                index = -1
                print "repeat:%s" % (aLine)
                for str in lines_seen:
                    index = index + 1
                    #print "index=%d str=%s aLine=%s" %(index, str, aLine)
                    if str == aLine:
                        array[index] = array[index] + 1
        else:
            done = 1
    for index,val in enumerate(array):
        if index < len(lines_seen):
            print 'index is %d, val is %d, string=%s' % (index,val,lines_seen[index])
    for item in lines_seen:
        oh.write(item)
    fh.close()
    oh.close()
    return

def joinPath(rootDir, fileName):
    return os.path.join(rootDir, fileName)

def usage():
    print "usage: libs.py -a ANDROID_DIR -d OUT_DIR -m MATCH_FILE -o OUT_FILE -s SET_FILE"
    print ""
    print "   param: ANDROID_DIR:   androir root directory"
    print "          OUT_DIR:       output the result directory"
    print "          MATCH_FILE:    save all the places match regular expression"
    print "          OUT_FILE:      write all the libsxx here"
    print "          SET_FILE:      parser OUT_FILE to set and write here"
    print ""
    print "   output :              $MATCH_FILE $OUT_FILE $SET_FILE in $OUT_DIR"

def  main():
    #./libs.py -r /media/frank/jam/linaro/aosp-M -d /media/frank/jam/linaro/LMG/LMG-914_static_link -m match.txt -o out.txt -s set.txt
    opts, args = getopt.getopt(sys.argv[1:], "hr:d:m:o:s:")
    for op, value in opts:
        if op == "-r":
            android_dir = value
        elif op == "-d":
            out_dir = value
            print out_dir
        elif op == "-m":
            match_file = joinPath(out_dir, value)
            print match_file
        elif op == "-o":
            out_file =  joinPath(out_dir, value)
            print out_file
        elif op == "-s":
            set_file = joinPath(out_dir, value)
            print set_file
        elif op == "-h":
            usage()
            sys.exit()
    search_opt(android_dir, match_file, out_file, set_file)

if __name__ == "__main__":
    main()

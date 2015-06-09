import sys
import os, os.path
import traceback
import glob
import argparse
sys.path.append("./Scripts")
from subprocess import call
import ntpath
import shutil
import zipfile


class Builder:
    def __init__(self):
        self.__genTypes = set()
        if (sys.platform == 'win32'):
            self.__genTypes.add('Visual Studio 12')
            self.__genTypes.add('Sublime Text 2 - Unix Makefiles')
            self.__curGenType = 'Visual Studio 12'
        elif (sys.platform == 'darwin'):
            self.__genTypes.add('Xcode')
            self.__genTypes.add('Unix Makefiles')
            self.__genTypes.add('Sublime Text 2 - Unix Makefiles')
            self.__curGenType = 'Xcode'
        else:
            # Linux
            self.__genTypes.add('Unix Makefiles')
            self.__genTypes.add('Sublime Text 2 - Unix Makefiles')
            self.__curGenType = 'Unix Makefiles'

    def ListTypes(self):
        for t in self.__genTypes:
            print t

    def Generate(self, genType=None):
        # Check if generation type is correct
        if genType:
            self.__curGenType = genType
        if not (self.__curGenType in self.__genTypes):
            raise Exception('Invalid generation type: ' + genType)

        if not self.__unpack('GLM'):
            print "Unable to generate project"
            return            
        # CMake generation
        print "Generating project " + self.__curGenType + "..."
        self.__outPath = './.build/' + self.__curGenType
        if not os.path.exists(self.__outPath) or not os.path.isdir(self.__outPath):
            os.makedirs(self.__outPath)
        oldWorkDir = os.getcwd()
        os.chdir(self.__outPath)
        result = call(['cmake', '-G', self.__curGenType, '../../'])
        os.chdir(oldWorkDir)
        if (result == 0):
            print "Generating project " + self.__curGenType + "...DONE"
        else:
            print "Generating project " + self.__curGenType + "...FAIL"

    def Build(self):
        print "INFO: Build is not implemented yet!"

    def Clean(self):
        shutil.rmtree(".build", ignore_errors=True)
        shutil.rmtree(".unpack", ignore_errors=True)

    def __unpack(self, module):
        # Unpack module
        print "Unpacking " + module + "..."
        currentDir = os.path.abspath(os.path.curdir)
        
        print currentDir
        srcPath = os.path.join(currentDir, "3rdParty", module, "glm.zip")
        if not os.path.exists(srcPath):
            print("Unpack failure: required module", module, "not found")
            return False
        dstPath = os.path.join(currentDir, ".unpack", module)
        if os.path.exists(dstPath) and os.path.isdir(dstPath):
            return True
            
        Zip = zipfile.ZipFile(srcPath)
        
        if Zip != None:
            Zip.extractall(dstPath)
            Zip.close()

        print "Unpacking " + module + "...DONE"
        return True        

    __curGenType = ''
    __genTypes = []
    __outPath = ''


def main():
    try:
        parser = argparse.ArgumentParser()
        parser.add_argument("-t", "--type",        help="Type of output for cmake", type=str)
        parser.add_argument("-l", "--list_types",  help="Lists generation type",    action="store_true")
        group = parser.add_mutually_exclusive_group()
        group.add_argument ("-b", "--build",       help="Build the project",        action="store_true")
        group.add_argument ("-c", "--clean",       help="Clean project",            action="store_true")
        args = parser.parse_args()

        builder = Builder()
        if args.list_types:
            builder.ListTypes()
            return

        if args.clean:
            builder.Clean()
            return

        builder.Generate(args.type)
        if args.build:
            builder.Build()

    except Exception:
        print traceback.format_exc()

main()

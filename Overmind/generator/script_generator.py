#!/usr/bin/python

import sys
import os
import argparse
import re

#keywords
__TABLE     = 'structure'
__METHOD    = 'metamethod'
__ENUM      = 'enumeration'

STL_TYPES = set(['string', 'vector', 'list', 'queue'])

#reserved field names
NAME_MARKER         = '__name'
FIELDS_MARKER       = '__fields'
METHOD_MARKER       = '__methods'
TYPE_MARKER         = '__type'
PARAMETERS_MARKER   = '__params'
ENUM_MARKER         = '__is_enum'

#general indices
SHORTCUTS = {'_header'              : '// This file is generated by python script.\n// Do not perform any manual modifications.',
             '_push'                : 'void PushObject(lua_State* L, const std::string& objectName,',
             '_parse'               : 'void ParseObject(lua_State* L, int index,',
             '_naming'              : 'if (objectName != "")\n        lua_setfield(L, -2, objectName.c_str());', 
             '_forbidMetatable'     : 'lua_pushcfunction(L, nullptr);\n    lua_setfield(L, -2, "__metatable");',
             '_forbidNewIndex'      : 'lua_pushcfunction(L, nullptr);\n    lua_setfield(L, -2, "__newindex");',
             '_registerThis'        : 'lua_pushlightuserdata(L, (void*)&pObject);\n    lua_setfield(L, -2, "__this");'}

# Functions

def checkTemplates(file, lineNumber, line):
    if re.search('(\w+)\s*<(.+)>', line):
        raise Exception('template detected in file %s @ ln%d: %s' % (file.name, lineNumber, line))

#############################################
#  HEADER FILE GENERATOR
#############################################

#all functions are templates. This required to push and parse complex template objects like std::vectors, etc.
def writeDependencies(headers, file):
    file.write('%(_header)s\n\n#include <internal/ScriptCommon.h>\n' % SHORTCUTS)
    for header in headers:
        file.write('#include <%s>\n' % header)

    file.write('\n')

#############################################
# raw tables
def generateParser(table, header, source):
    header.write("%(_parse)s %(__name)s& table);\n" % table)

    source.write("""
%(_parse)s %(__name)s& table)
{
    index;

    luaL_checktype(L, -1, LUA_TTABLE);
    lua_pushnil(L);
    while(lua_next(L, -2))
    {
        std::string field(lua_tostring(L, -2));""" % table)

    for record in table[FIELDS_MARKER]:
        source.write("""
        if (0 == field.compare(\"%(__name)s\"))
        {
            ParseObject(L, -1, table.%(__name)s);
            lua_pop(L, 1);
            continue;
        }""" % record)

    source.write("""
        RaiseError(L, "unknown field \\"" + field + "\\" found");
        lua_pop(L, 1);
    } /* while */
}
""" % table)

# push function
def generatePush(table, header, source):
    header.write("%(_push)s %(__name)s& table);\n" % table)
    source.write("\n%(_push)s %(__name)s& table)\n{\n    lua_newtable(L);\n" % table)
    for record in table[FIELDS_MARKER]:
        source.write('\n    PushObject(L, "%(__name)s", table.%(__name)s);' % record)

    source.write('\n\n    %(_naming)s\n}\n' % table)

#############################################
# enumerations. All enumerations are threated as table of ints
# register function
def generateEnumRegistrator(table, header, source):
    source.write("""
void RegisterEnum_%(__name)s(lua_State* L)
{
    lua_getglobal(L, "%(__name)s");
    if (!lua_isnil(L, -1))
        RaiseError(L, "enumerator %(__name)s already registered");

    lua_newtable(L);

    %(_forbidMetatable)s
    %(_forbidNewIndex)s\n
""" % table)
    #register enumerator fields
    for record in table[FIELDS_MARKER]:
        source.write('    PushObject(L, "%s", (int)%s::%s);\n' % (record[NAME_MARKER], table[NAME_MARKER], record[NAME_MARKER]))

    #enums are allways registered in global field
    source.write('    lua_setglobal(L, "%(__name)s");\n}\n' % table)

#############################################
# interface
def generateFunctionWrappers(table, header, source):
    for method in table[METHOD_MARKER]:
        source.write('\nint lua%s_%s(lua_State* L)\n{\n' % (table[NAME_MARKER], method[NAME_MARKER]))
        # validate number of input parameters
        source.write('    if (lua_gettop(L) != %d )\n        RaiseError(L, "invalid number of arguments for %s::%s");\n' % (len(method[PARAMETERS_MARKER]) + 1, table[NAME_MARKER], method[NAME_MARKER]))
        source.write("""
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_getfield(L, 1, "__this");
    if (!lua_islightuserdata(L, -1))
        RaiseError(L, "invalid first argument of the method %(__name)s");

    %(__name)s* pThis = (%(__name)s*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    if (!pThis)
        RaiseError(L, "incorrect method self parameter");

""" % table)
        # get parameters from LUA stack
        retvalsCount = 0 if method[TYPE_MARKER] == 'void' else 1;
        for index, parameter in enumerate(method[PARAMETERS_MARKER]):
            param = parameter
            param.update({'__index' : index + 2})
            source.write("    %(__type)s %(__name)s; ParseObject(L, %(__index)d, %(__name)s);\n" % param)

        # start native function generation
        source.write('    PushObject(L, "", pThis->%(__name)s(' % method) if 0 != retvalsCount else source.write('    pThis->%(__name)s(' % method)

        # push parameters as arguments of native function
        for index, parameter in enumerate(method[PARAMETERS_MARKER]):
            source.write('%(__name)s, ' % parameter) if index < len(method[PARAMETERS_MARKER])-1 else source.write('%(__name)s' % parameter)

        # end of function
        source.write(');\n') if retvalsCount == 0 else source.write(') );\n') ; 
        source.write('    return %d;\n}\n' % retvalsCount)

def generatePushObject(table, header, source):
    header.write('%(_push)s %(__name)s& pObject);\n%(_parse)s %(__name)s& value);\n' % table)
    #generate interface registration. interface table is closed, no fields can be assigned
    source.write('\n%(_push)s %(__name)s& pObject)\n{\n    lua_newtable(L);\n    %(_forbidMetatable)s\n    %(_forbidNewIndex)s\n    %(_registerThis)s\n' % table)

    #register methods
    for method in table[METHOD_MARKER]:
        source.write('    lua_pushcfunction(L, lua%s_%s); lua_setfield(L, -2, "%s");\n' % (table[NAME_MARKER], method[NAME_MARKER], method[NAME_MARKER]))

    source.write('\n    %(_naming)s\n}\n' % table)
    #generate parse function for interface object
    source.write("""
%(_parse)s %(__name)s& value)
{
    index;
    luaL_checktype(L, -1, LUA_TTABLE);
    lua_getfield(L, -1, "__this");
    if (!lua_islightuserdata(L, -1))
        RaiseError(L, "invalid first argument of the method %(__name)s");

    value = *(%(__name)s*)lua_touserdata(L, -1);
    lua_pop(L, 1);
}
""" % table)

# globals generation

def generateGlobalsRegistration(tableList, header, source):
    header.write('void RegisterGlobals(lua_State* L);\n')
    source.write('void RegisterGlobals(lua_State* L)\n{\n')

    for enum in tableList:
        source.write('    RegisterEnum_%(__name)s(L);\n' % enum)
    source.write('}\n')

# source file itself
def generateSourceFile(tableList, header, source):
    if source.name != header:
        source.write('%s\n\n#include "%s"\n' % (SHORTCUTS['_header'], header.name))

    enums = []
    for table in tableList:
        mergedTable = table
        mergedTable.update(SHORTCUTS)

        source.write('\n///// %(__name)s\n' % table);

        if table[ENUM_MARKER]:
            generateEnumRegistrator(mergedTable, header, source)
            enums.append(mergedTable)

        else:
            if len(table[METHOD_MARKER]) > 0:
                generateFunctionWrappers(mergedTable, header, source)
                generatePushObject(mergedTable, header, source)

            elif len(table[FIELDS_MARKER]) > 0:
                generateParser(mergedTable, header, source)
                generatePush(mergedTable, header, source)
        header.write('\n')

    generateGlobalsRegistration(enums, header, source)

#############################################
#  TABLE PARSER
#############################################
def main():
    parser = argparse.ArgumentParser(description = 'C++ to Lua converter v0.1')
    parser.add_argument('-fl',  '--file_list',  required = True, type = argparse.FileType('r'), help = 'list of files to be processed', nargs = '+')
    parser.add_argument('-ho',  '--header',     required = True, type = argparse.FileType('w'), help = 'output header file name')
    parser.add_argument('-so',  '--source',     required = True, type = argparse.FileType('w'), help = 'output source file name')
    scriptArgs = parser.parse_args()

    tableList = []

    methodPattern = re.compile("""%(__METHOD)s\s+               # 'metamethod' marker is mandatory
        (?P<%(TYPE_MARKER)s>\w+)\s+(?P<%(NAME_MARKER)s>\w+)\s*  # function return type and name
        \(\s*
        (?P<%(PARAMETERS_MARKER)s>.+)*                          # function parameters
        \s*\)
        \s*=\s*0\s*;                                            # PURE marker is mandatory
        """ % globals(),
        re.VERBOSE)

    #TODO:
    # allow pointer registration in structure
    fieldPattern = re.compile('(?P<%(TYPE_MARKER)s>\w+)\s+(?P<%(NAME_MARKER)s>\w+)\s*;' % globals())
    enumPatter = re.compile("""
        (?P<%(NAME_MARKER)s>\w+)                     # enumerated value name
        (?:\s*=\s*.*)?                  # enum assigment is optional, symbol group is not captured
        \,?                             # comma is optional
        """ % globals(), 
        re.VERBOSE) # enumeration field with optional assignment

    currentTable = {}
    filesList = set([])
    for file in scriptArgs.file_list:
        lineNumber = 0

        tableStarted = False
        enumeration = False
        for line in file:
            lineNumber += 1
            line = re.sub('\s+', ' ', line)       # replace complex spaces by simple one
            line = re.sub('\/\/.*', '', line)     # remove single line comments
            line = re.sub('std\:\:', '', line)    # remove std:: prefixes
            #TODO:
            # undertand how multiple subline comments can be removed.
            # understand multiline comments as well
            line = re.sub('\/\*.*\*\/', '', line) # remove single subline comment
            line = line.strip()

            if line is '':
                continue

            index = 0
            if not tableStarted:
                tableName = re.match('%s\s+(?P<%s>\w+)' % (__TABLE, NAME_MARKER), line) or re.match('%s\s+(?P<%s>\w+)' % (__ENUM, NAME_MARKER), line)
                if tableName is not None:
                    enumeration = (re.search(__ENUM, line) != None)
                    currentTable = {NAME_MARKER : tableName.groupdict()[NAME_MARKER], FIELDS_MARKER : [], METHOD_MARKER : [], ENUM_MARKER : enumeration}
                    tableStarted = True
                    line = line[tableName.span()[1]:].strip()
                    filesList.add(file.name)
                else:
                    continue

            parameter = re.search("{",line)
            if parameter is not None:
                line = line[parameter.span()[1]:].strip()

            parameter = fieldPattern.match(line) if not enumeration else enumPatter.match(line)
            # field detected. parse it and save as a tuple (type, name)
            if parameter is not None and len(parameter.groups()) > 0:
                # check template in translated field. non translated fields may be templates
                if not parameter.groupdict().has_key(TYPE_MARKER):
                    currentTable[FIELDS_MARKER].append( {TYPE_MARKER: 'int', NAME_MARKER: parameter.groupdict()[NAME_MARKER] } )
                else:
                    checkTemplates(file, lineNumber, line)
                    currentTable[FIELDS_MARKER].append( parameter.groupdict() )
            else:
                # function detected
                # Save function as a name, typoe and list of parameters.
                parameter = methodPattern.match(line)
                if parameter is not None and len(parameter.groups()) > 1:
                    # check template in translated method. non translated methods may be templates
                    checkTemplates(file, lineNumber, line)
                    funcParams = []
                    #TODO:
                    # const parameters are not allowed yet
                    funcDesc = parameter.groupdict()
                    if funcDesc[PARAMETERS_MARKER] != None:
                        for param in funcDesc[PARAMETERS_MARKER].split(','):
                            checkTemplates(file, lineNumber, param.strip())
                            param = re.sub('\s*const\s+', '', param)
                            param = re.sub('\&', '', param)
                            funcParameter = param.strip().split(' ')
                            if  funcParameter[0] in STL_TYPES:
                                funcParameter[0] = 'std::' + funcParameter[0]
                            funcParams.append( {TYPE_MARKER: funcParameter[0], NAME_MARKER: funcParameter[1]} )

                    funcDesc[PARAMETERS_MARKER] = funcParams
                    currentTable[METHOD_MARKER].append(funcDesc)

            if re.search("}", line) is not None:
                tableList.append(currentTable)
                currentTable = {}
                tableStarted = False
                enumeration  = False

    writeDependencies(filesList, scriptArgs.header)
    generateSourceFile(tableList, scriptArgs.header, scriptArgs.source)


if __name__=="__main__":
    main()


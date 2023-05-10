import os
import re
import sys

header_template ="""#pragma once

#include <string>
#include <vulkan/vulkan.hpp>
"""

source_template ="""
#include "vulkan_enum_to_string.h"

"""

decl_template ="""std::string toString({0} v);\n"""

decl_ns_template ="""namespace {0}_
{{
std::string toString({0} v);
}}
"""

impl_template ="""std::string toString(const {0} v)
{{
    switch (v)
    {{
{1}
    }}
    return "";
}}
"""

impl_flag_template ="""std::string toString(const {0} v)
{{
    std::string s;
    {1}
    return s;
}}
"""

impl_flag_ns_template ="""namespace {0}_
{{
std::string toString(const {0} v)
{{
    std::string s;
    {1}
    return s;
}}
}}
"""

def string_to_int(s):
    if 'x' in s:
        return True, int(s, 16)
    try:
        return True, int(s)
    except:
        return False, -1

if __name__ == "__main__":
    with open(sys.argv[1], 'r') as f:
        text = ''.join(f.readlines())
    
    # Pattern for matching an entire enum.
    pattern0 = re.compile('typedef enum(\w|\s|{|}|=|,|#|-)*;')
    # Pattern for matching an enum value.
    pattern1 = re.compile('\w+\s*=\s*-?[a-zA-Z0-9]+')
    # Pattern for getting the enum name.
    pattern2 = re.compile('\w+;')
    
    structs = []
    start = 0
    while True:
        match = pattern0.search(text, start)
        if match:
            structs.append((match.start(), match.end()))
            start = match.end()
        else:
            break
    
    os.makedirs(sys.argv[2], exist_ok=True)
    header_file = open(sys.argv[2] + 'vulkan_enum_to_string.h', 'w')
    source_file = open(sys.argv[2] + 'vulkan_enum_to_string.cpp', 'w')
    header_file.write(header_template)
    source_file.write(source_template)
    
    namespace = False
    
    for s in structs:
        struct = text[s[0]:s[1]]
        lines = struct.splitlines()
        match = pattern2.search(struct)
        enum_name = struct[match.start():match.end() - 1]
        
        if 'FlagBits' in enum_name:
            enum_name = enum_name.replace('FlagBits', 'Flags')
            
            ifs = ''
            start = 0
            values = []
            for line in lines:
                match = pattern1.search(line)
                if not match:
                    if '#ifdef' in line or '#endif' in line:
                        ifs += line + '\n'
                    continue
                
                enum = line[match.start():match.end()].split('=')
                if len(enum) != 2:
                    continue

                value_name = enum[0].strip()
                is_int, value = string_to_int(enum[1].strip())
                if not is_int:
                    continue
                
                if value in values:
                    continue
                
                ifs += '    if (v & {}) s += std::string("{}+");\n'.format(value_name, value_name)
                values.append(value)
            
            # Write function definition.
            if re.search('typedef(\w|\s)+{0};'.format(enum_name), text):
                source_file.write(impl_flag_ns_template.format(enum_name, ifs))
                namespace = True
            else:
                source_file.write(impl_flag_template.format(enum_name, ifs))
        else:
            cases = ''
            start = 0
            values = []
            for line in lines:
                match = pattern1.search(line)
                if not match:
                    if '#ifdef' in line or '#endif' in line:
                        cases += line + '\n'
                    continue
                
                enum = line[match.start():match.end()].split('=')
                if len(enum) != 2:
                    continue

                value_name = enum[0].strip()
                is_int, value = string_to_int(enum[1].strip())
                if not is_int:
                    continue
                
                if value in values:
                    continue
                
                cases += '        case {}: return "{}";\n'.format(value_name, value_name)
                values.append(value)
            
            # Write function definition.
            source_file.write(impl_template.format(enum_name, cases))
        
        # Write function declaration.
        if namespace:
            header_file.write(decl_ns_template.format(enum_name))
        else:
            header_file.write(decl_template.format(enum_name))

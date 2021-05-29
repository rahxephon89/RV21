
#!/usr/bin/env python3

import sys
import os

if len(sys.argv) != 3 and len(sys.argv) != 2:
    print('wrong args number')
    sys.exit();

input_file = sys.argv[1]
flag = False
num = 0
if len(sys.argv) == 3:
   num = int(sys.argv[2]) 
   flag = True
#output_file = .argv[1]

with open('./'+input_file) as fp:
    i = 0
    for line in fp:
        i += 1
        lst = line.split(',')
        #task = int(arg0_str.split(' = ')[1])
        #resource = int(arg1_str.split(' = ')[1])
        event = ''
        ev_name = lst[0]
        if ev_name == 'create_auction':
            channel = 'ch1'
            item = int(lst[1][4:])
            bid = int(lst[2])
            time = int(lst[3])
            event = f"""{{
                "fmt_version": [2, 0],
                "channel": "{channel}",
                "event": "{ev_name}",
                "params": [{item}, {bid},{time}],
                "aux": {{"line": {i}}}
                }}"""
        elif ev_name == 'bid':
            channel = 'ch2'
            item = int(lst[1][4:])
            bid = int(lst[2])
            event = f"""{{
                "fmt_version": [2, 0],
                "channel": "{channel}",
                "event": "{ev_name}",
                "params": [{item}, {bid}],
                "aux": {{"line": {i}}}
                }}"""
        elif ev_name == 'sold':
            channel = 'ch3'
            item = int(lst[1][4:])
            event = f"""{{
                "fmt_version": [2, 0],
                "channel": "{channel}",
                "event": "{ev_name}",
                "params": [{item}],
                "aux": {{"line": {i}}}
                }}"""
        elif ev_name == 'endOfDay':
            channel = 'ch4'
            event = f"""{{
                "fmt_version": [2, 0],
                "channel": "{channel}",
                "event": "{ev_name}",
                "params": [],
                "aux": {{"line": {i}}}
                }}"""     
        else:
            raise ValueError(f"Unknown event name {ev_name}")
        
        
        print(event)
        if flag:
            if num == 0:
                break
            else:
               num -= 1

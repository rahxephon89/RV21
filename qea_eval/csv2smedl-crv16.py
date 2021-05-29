
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
        quote = '\"'
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
        elif ev_name == 'input':
            channel = 'ch1'
            sq = int(lst[1].strip()[13:],16)
            event = f"""{{
                "fmt_version": [2, 0],
                "channel": "{channel}",
                "event": "{ev_name}",
                "params": [{sq}],
                "aux": {{"line": {i}}}
                }}"""
        elif ev_name == 'sanitise':
            channel = 'ch4'
            sq = int(lst[1].strip()[13:],16)
            event = f"""{{
                "fmt_version": [2, 0],
                "channel": "{channel}",
                "event": "{ev_name}",
                "params": [{sq}],
                "aux": {{"line": {i}}}
                }}"""
        elif ev_name == 'derive':
            channel = 'ch2'
            sq1 = int(lst[1].strip()[13:],16)
            sq2 = int(lst[2].strip()[13:],16)
            event = f"""{{
                "fmt_version": [2, 0],
                "channel": "{channel}",
                "event": "{ev_name}",
                "params": [{sq1},{sq2}],
                "aux": {{"line": {i}}}
                }}"""
        elif ev_name == 'use':
            channel = 'ch3'
            sq = int(lst[1].strip()[13:],16)
            event = f"""{{
                "fmt_version": [2, 0],
                "channel": "{channel}",
                "event": "{ev_name}",
                "params": [{sq}],
                "aux": {{"line": {i}}}
                }}"""
        elif ev_name == 'member':
            channel = 'ch1'
            v = lst[1].strip()
            p = lst[2].strip()
            event = f"""{{
                "fmt_version": [2, 0],
                "channel": "{channel}",
                "event": "{ev_name}",
                "params": [{quote}{v}{quote},{quote}{p}{quote}],
                "aux": {{"line": {i}}}
                }}""" 
        elif ev_name == 'candidate':
            channel = 'ch2'
            v = lst[1].strip()
            c = lst[2].strip()
            event = f"""{{
                "fmt_version": [2, 0],
                "channel": "{channel}",
                "event": "{ev_name}",
                "params": [{quote}{v}{quote},{quote}{c}{quote}],
                "aux": {{"line": {i}}}
                }}""" 
        elif ev_name == 'end':
            channel = 'ch3'
            event = f"""{{
                "fmt_version": [2, 0],
                "channel": "{channel}",
                "event": "{ev_name}",
                "params": [],
                "aux": {{"line": {i}}}
                }}""" 
        elif ev_name == 'rank':
            channel = 'ch7'
            v = lst[1].strip()
            c = lst[2].strip()
            r = int(lst[3].strip())
            event = f"""{{
                "fmt_version": [2, 0],
                "channel": "{channel}",
                "event": "{ev_name}",
                "params": [{quote}{v}{quote},{quote}{c}{quote}, {r}],
                "aux": {{"line": {i}}}
                }}"""        
        else:
            print(ev_name)
            raise ValueError(f"Unknown event name {ev_name}")
        
        
        print(event)
        if flag:
            if num == 0:
                break
            else:
               num -= 1

Case studies come from [CRV'16](https://crv.liflab.ca/wiki/index.php/Main_Page)

[Description for the Auction example](https://crv.liflab.ca/wiki/index.php/Offline_Team2_Benchmark1)

[Description for the Candidate Selection example](https://crv.liflab.ca/wiki/index.php/Offline_Team2_Benchmark2)

The Auction example is tested against the trace *auction_trace_invalid1* and its sub trace.

The Candidate example is tested against the trace  *cand_sel_invalid2* and its sub trace.

The user can use *csv2smedl-crv16.py* to generate the json trace for SMEDL monitors.

To generate C code in the *targetdir* from a SMEDL specification with an architecture file (with *a4smedl* as the file extension), use the command "mgen -d *targetdir* -t file *a4smedl file*". Generated cod

To compile the generated code into executable, use the make command. 

To run the executable *mon*, use the command "*mon -- tracefile*"

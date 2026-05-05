

Compile with 

g++  "CMParser.cpp" "document.cpp" "parser.cpp" -I "./" -o "CMParser"

Outputs:

Valid files:

artan@IT15:~/work/cmparser$ ./CMParser valid/valid_01_root_plain_text.cmml
Root text only
artan@IT15:~/work/cmparser$ ./CMParser valid/valid_02_bfs_not_dfs.cmml
Alpha shallow
Charlie shallow
Bravo deep one
Bravo deep two
artan@IT15:~/work/cmparser$ ./CMParser valid/valid_03_more_bfs.cmml
East Gate
South Dock
North Market
Clock Tower
Glass Depot
West Harbor
Old Mill
artan@IT15:~/work/cmparser$ ./CMParser valid/valid_04_whitespace_between_tags.cmml
First
Fourth
Second
Third
artan@IT15:~/work/cmparser$ ./CMParser valid/valid_05_inner_tag_same_as_outer.cmml
Shift A summary
End of report
Door
Open
High
Vault
Locked
Normal
Roof
Clear
Low
Window
Ceiling
artan@IT15:~/work/cmparser$ ./CMParser valid/valid_06_newline_inside_text.cmml
Galaxy Survey
Draft two
Arrival
Harbor
      bells ring at dawn
Scouts carry lanterns into fog
Decision
The council chooses the northern road
A messenger waits by the gate
artan@IT15:~/work/cmparser$ ./CMParser valid/valid_07_compact_single_line.cmml
Middle shallow
Left leaf
Right leaf
artan@IT15:~/work/cmparser$ ./CMParser valid/valid_08_random_whitespace.cmml
Operations Dashboard
Active
Admin
Guest
Create
Read
Update
Read


Invalid files:

artan@IT15:~/work/cmparser$ ./CMParser invalid/invalid_01_wrong_root.cmml
Error line 1 column 2
artan@IT15:~/work/cmparser$ ./CMParser invalid/invalid_02_text_before_root.cmml
Error line 1 column 1
artan@IT15:~/work/cmparser$ ./CMParser invalid/invalid_03_text_after_root.cmml
Error line 1 column 16
artan@IT15:~/work/cmparser$ ./CMParser invalid/invalid_04_mismatched_close.cmml
Error line 1 column 26
artan@IT15:~/work/cmparser$ ./CMParser invalid/invalid_05_crossing_scope.cmml
Error line 1 column 18
artan@IT15:~/work/cmparser$ ./CMParser invalid/invalid_06_unclosed_outer_before_doc_close.cmml
Error line 1 column 34
artan@IT15:~/work/cmparser$ ./CMParser invalid/invalid_07_text_then_child.cmml
Error line 1 column 20
artan@IT15:~/work/cmparser$ ./CMParser invalid/invalid_08_child_then_text.cmml
Error line 1 column 32
artan@IT15:~/work/cmparser$ ./CMParser invalid/invalid_09_less_than_in_text.cmml
Error line 1 column 8
artan@IT15:~/work/cmparser$ ./CMParser invalid/invalid_10_greater_than_in_text.cmml
Error line 1 column 8
artan@IT15:~/work/cmparser$ ./CMParser invalid/invalid_11_slash_in_text.cmml
Error line 1 column 8
artan@IT15:~/work/cmparser$ ./CMParser invalid/invalid_12_slash_in_label.cmml
Error line 1 column 10
artan@IT15:~/work/cmparser$ ./CMParser invalid/invalid_13_missing_root_close.cmml
Error line 1 column 21
artan@IT15:~/work/cmparser$ ./CMParser invalid/invalid_14_multiline_mixed_scope.cmml
Error line 4 column 5
artan@IT15:~/work/cmparser$ ./CMParser invalid/invalid_15_multiline_slash_in_label.cmml
Error line 3 column 7
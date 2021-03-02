(gci -include *.cs,*.cpp,*.h,*.vert,*.frag -recurse | select-string .).Count
PAUSE
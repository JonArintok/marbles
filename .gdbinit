

define tfld
	run test/foldability.marbles
end

break parse.h:314

define nnp
	next
	echo curNode....
	output curNode
	echo \n
	echo name.......
	output nodesInfo[curNode].name
	echo \n
	echo line.......
	output nodesInfo[curNode].line
	echo \n
	echo prevDelim..
	output prevDelim
	echo \n
	echo \n
end




define listNodes
	set $i = $arg0
	while $i <= $arg1
		echo \n
		echo node:
		output $i
		echo \n
		echo name........
		output nodesInfo[$i].name
		echo \n
		echo line........
		output nodesInfo[$i].line
		echo \n
		echo level.......
		output nodesInfo[$i].level
		echo \n
		echo childCount..
		output nodes[$i].childCount
		echo \n
		echo children....
		output nodes[$i].children
		echo \n
		echo evaluate....
		output nodes[$i].evaluate
		echo \n
		echo output.n....
		output nodes[$i].output.n
		echo \n
		set $i = $i + 1
	end
end

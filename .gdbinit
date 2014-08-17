

define tfld
	run test/foldability.marbles
end

break marblerun.c:35


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

define stepAndPrint
	next
	echo name........
	output nodesInfo[currentNode].name
	echo \n
	echo level.......
	output nodesInfo[currentNode].level
	echo \n
	echo childCount..
	output nodes[currentNode].childCount
	echo \n
	echo children....
	output nodes[currentNode].children
	echo \n
	echo evaluate....
	output nodes[currentNode].evaluate
	echo \n
	echo output.n....
	output nodes[currentNode].output.n
	echo \n
end



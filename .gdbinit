
define listNodes
	set $i = 0
	while $i <= currentNode
		echo \n
		echo node:
		output $i
		echo \n
		echo name........
		output nodesInfo[$i].name
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


break parse.h:596

define tfld
	run test/foldability.marbles
end

define stepAndPrint
	next
	echo name.......
	print nodesInfo[currentNode].name
	echo level......
	print nodesInfo[currentNode].level
	echo fold.......
	print fold
	echo prevDelim..
	print prevDelim
end





define pNameAndLevel
	echo name...
	print nodesInfo[$arg0].name
	echo level..
	print nodesInfo[$arg0].level
end

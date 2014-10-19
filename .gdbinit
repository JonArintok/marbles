

define test
	echo running test/functionPass.marbles...
	echo \n
	run test/functionPass.marbles
end

break resolveNode

define pstack
	set $i = 0
	while $i <= stackPos
		echo \n
		output $i
		echo \n
		output stack[$i].n
		echo \n
		set $i = $i + 1
	end
end

define cnp
	c
	echo \n
	echo self.......
	output self
	echo \n
	echo line.......
	output nodesInfo[self].line
	echo \n
	echo stackPos...
	output stackPos
	echo \n
	echo stackOut...
	output stack[stackPos].n
	echo \n
end

define pnode
	echo node:
	output $arg0
	echo \n
	echo name........
	output nodesInfo[$arg0].name
	echo \n
	echo line........
	output nodesInfo[$arg0].line
	echo \n
	echo level.......
	output nodesInfo[$arg0].level
	echo \n
	echo frameform...
	output nodesInfo[$arg0].frameform
	echo \n
	echo definition..
	output nodes[$arg0].definition
	echo \n
	echo argRefIndex.
	output nodes[$arg0].argRefIndex
	echo \n
	echo childCount..
	output nodes[$arg0].childCount
	echo \n
	echo children....
	output nodes[$arg0].children
	echo \n
	echo evaluate....
	output nodes[$arg0].evaluate
	echo \n
	echo output.n....
	output nodes[$arg0].cache.n
	echo \n
end
	

define pnodes
	set $i = $arg0
	while $i <= $arg1
		pnode $i
		echo \n
		set $i = $i + 1
	end
end

define nnp
	next
	pnode curNode
end


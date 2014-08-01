

define printNode
	echo name\t
	print nodes[$arg0].name
	echo definition\t
	print nodes[$arg0].definition
	echo argRefIndex\t
	print nodes[$arg0].argRefIndex
	echo arity\t
	print nodes[$arg0].arity
	echo evaluate\t
	print nodes[$arg0].evaluate
end


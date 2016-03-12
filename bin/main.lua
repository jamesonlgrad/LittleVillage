elapse = 0

wordlist = {  }
maxword = 0

poemlist = {  }
curpoem = 1
maxpoem = 0

storylist = {  }
curstory = 0
maxstory = 0

function loadfile(filename)
	local a = {}
	local amax = 0
	local file = io.open(filename, "r")
	for line in file:lines() do
		table.insert(a, line)
		amax = amax + 1
	end
	file:close()
	return a, amax
end

function random_word()
	return wordlist[math.random(maxword)]
end

function next_poem()
	local opt = " "
	if(math.random(100) == 1) then
		opt = poemlist[curpoem]
		curpoem = curpoem + 1
	end
	return opt
end

function next_story()
	curstory = curstory + 1
	return storylist[curstory]
end

function start()
	math.randomseed(os.time())
	wordlist, maxword = loadfile("words2.txt")
	poemlist, maxpoem = loadfile("poem.txt")
	storylist, maxstory = loadfile("story.txt")
end

function update()
	elapse = elapse + 1
end
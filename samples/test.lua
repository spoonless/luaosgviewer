local monkey = entity.load("monkey.osgt")
monkey.name = "my monkey"

monkey:addEventHandlers{
    focusin = function(self)
        print("focus in " .. self.name)
    end,
}

local otherMonkey = monkey:clone();
otherMonkey.name = "another monkey (but not mine)"

local monkeys = {
    [{x=5,y=0,z=0}] = monkey,
    [{-5,0,0}] = otherMonkey,
}

local axes = entity.load("axes.osgt")

return axes, monkeys

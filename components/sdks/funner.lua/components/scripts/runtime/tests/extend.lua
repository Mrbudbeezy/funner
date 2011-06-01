print ("Results of extend_test:")

require "com.untgames.runtime"

local function CreateClassA ()
  local function Foo ()
    print ("ClassA.Foo")
  end  
  
  local function SetProperty (name, value)
    print (string.format ("SetProperty('%s', '%s')", name, value))
  end
  
  local function GetProperty (name)
    print (string.format ("GetProperty('%s')", name))
  end
  
  local function Create ()
    local function Baz ()
      print ("ClassA.Baz")
    end

    return class { Baz = Baz }
  end

  return class "ClassA" { Foo = Foo, set_A = SetProperty, get_A = GetProperty, Create = constructor (Create)}
end

local ClassA = CreateClassA ()

local function CreateClassB ()
  local function Bar ()
    print ("ClassB.Bar")
  end    
  
  local function Create ()
    local function Foo ()
      print ("ClassB.Foo")
    end

    return class { extend (ClassA.Create ()), Foo = Foo }
  end

  return class "ClassB" { extend (ClassA), Bar = Bar, Create = constructor (Create)}
end

local ClassB = CreateClassB ()

local instance = ClassB.Create ()

instance.Foo ()
instance.Bar ()
instance.Baz ()

instance.A = 1

local x = instance.A

print (string.format ("Instance name: '%s'", tostring (instance)))
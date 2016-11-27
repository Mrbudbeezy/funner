function test_vector (v)
  print ("Input:     " .. tostring (v))
  print ("Get v.x:   " .. tostring (v.x))
  print ("Get v.y:   " .. v.y)
  print ("Get v.z:   " .. v.z)

  print ("Get v.xzy: " .. tostring (v.xzy))

  tmp = v.xyz + v.yzx

  print ("tmp = v.xyz + v.yzx: " .. tostring (tmp))

  tmp = tmp - v * v.zxy

  print ("tmp = tmp - v * v.zxy: " .. tostring (tmp))
  print ("v * 2: " .. tostring (v * 2))
  print ("3 * v: " .. tostring (3 * v))
  print ("4 * v / 2: " .. tostring (4 * v / 2))

  print ("vec3(4,5,6): " .. tostring (vec3 (4, 5, 6)))

  v1 = vec3 (7)

  print ("v1 = vec3(7): " .. tostring (v1))
  print ("v1:normalize(): " .. tostring (v1:normalize()))

  v1 = vec3 (-1, 2, -3):abs ()

  print ("v1 = vec3 (-1, 2, -3):abs (): " .. tostring (v1))

  v2 = v1:min (tmp)

  print ("v2 = v1:min (tmp): " .. tostring (v2))

  v2 = v1:max (tmp)

  print ("v2 = v1:max (tmp): " .. tostring (v2))

  v3 = v1:cross (v2)

  print ("v3 = v1:cross (v2): " .. tostring (v3))
  print ("v3.qlength: " .. v3.qlength)

  v1 = v.xyz -- явное копирование, а не создание ссылки на объект

  v1 [0] = 4
  v1 [1] = 5
  v1.z   = 6

  print ("After set values: " .. tostring (v1))

end

function test_matrix (m)
  print ("Input:   ");

  print (m [0])
  print (m [1])
  print (m [2])
  print (m [3])

  m [0].zxy = vec3 (1, 2, 3)

  print ("After m[0].zxy=vec3(1,2,3): " .. tostring (m [0]))

  local m1 = mat4 (2)
  local m2 = mat4 (3)
  local m3 = m1 * m2

  print ("Result of matrix multiplies: " .. tostring (m3))

  m3:set (1, 1, 123)

  print ("After m3:set(1,1,123): " .. tostring (m3))

  m3 [2][2] = 456

  print ("After m3 [2][2]=456: " .. tostring (m3))

  local m5 = mat4 (2)

  print ("mat4(2):det (): " .. m5:det ())

  print ("m5 * m5: " .. tostring (m5 * m5))
  print ("m5 * 2: " .. tostring (m5 * 2))
  print ("3 * m5: " .. tostring (3 * m5))
  print ("4 * m5 / 2: " .. tostring (4 * m5 / 2))
  print ("m5 * vec3 (1, 2, 3): " .. tostring (m5 * vec3 (1, 2, 3)))
  print ("m5 * vec4 (1, 2, 3, 4): " .. tostring (m5 * vec4 (1, 2, 3, 4)))
  print ("vec3 (1, 2, 3) * m5: " .. tostring (vec3 (1, 2, 3) * m5))
  print ("vec4 (1, 2, 3, 4) * m5: " .. tostring (vec4 (1, 2, 3, 4) * m5))
end

function test_quat (q)
  print ("Input: " .. tostring (q))

  local q1 = q

--  math.quat.set (q1, 1, 2)

  q1:set (1, 2)

  print ("After q1:set(1,2): " .. tostring (q1))

  for i=0,3 do
    print ("      q:get(" .. i .. "): " .. q:get (i))
  end

  for i=0,3 do
    print ("      q [" .. i .. "]: " .. q [i])
  end

  print ("      q.x:" .. q.x)
  print ("      q.y:" .. q.y)
  print ("      q.z:" .. q.z)
  print ("      q.w:" .. q.w)
  
  print ("Test q.{x,y,z,w}={5,6,7,8}:")

  q.x = 5
  q.y = 6
  q.z = 7
  q.w = 8
  
  for i=0,3 do
      print (" q["..i.."]:"..tostring(q[i]))
  end
  
  local q2 = quat (8, 9, 10, 11)
  
  print ("Test q2=quat(8,9,10,11): " .. tostring (q2))
  print ("Test quat(1): " .. tostring (quat (1)))

  print ("Test -q2: " .. tostring (-q2))
  
  print ("Test q+q2:"..tostring(q+q2))
  
  print ("Test q-q2:"..tostring(q-q2))
  
  print ("Test q2*q2:"..tostring(q2*q2))

  print ("Test q2:normalize(): "..tostring(q2:normalize()))
  
  print ("Test q2:inner(q): "..tostring(q2:inner(q)))
end

function test_utility ()
  print ("translate " .. tostring (Math.translate (vec3 (1, 2, 3))))
  print ("rotate " .. tostring (Math.rotate (90, vec3 (0, 0, 1))))
  print ("scale " .. tostring (Math.scale (vec3 (1, 2, 3))))
  print ("lookat " .. tostring (Math.lookat (vec3 (1, 0, 0), vec3 (0), vec3 (0, 1, 0))))

  local q = Math.to_quat (90, vec3 (0, 0, 1));
  
  print (string.format ("q = [%.3f %.3f %.3f %.3f] ", q.x, q.y, q.z, q.w))
  print ("to_matrix (q) " .. tostring (Math.to_matrix (q)))
  
  q = Math.to_quat (90, 0, 0)
  
  print (string.format ("q = [%.3f %.3f %.3f %.3f] ", q.x, q.y, q.z, q.w))
  
  q = Math.to_quat (Math.to_matrix (q))
  
  print (string.format ("q = [%.3f %.3f %.3f %.3f] ", q.x, q.y, q.z, q.w))

end

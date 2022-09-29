base = 20000.0
maxt = 10.0
attack = 0.3
st = 1.0/44100.0
env = 0.0
x = 0

while env < 1.0:
    env = env + (base**(1-attack)) / maxt * (1.01 - env) * st
    x = x + 1
    print(env)
print(x)
	
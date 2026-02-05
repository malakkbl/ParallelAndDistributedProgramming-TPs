import matplotlib.pyplot as plt

p = [1, 2, 4, 8, 16, 32, 64]
fs = 0.303

amdahl = [1 / (fs + (1 - fs) / x) for x in p]
gustafson = [x - fs * (x - 1) for x in p]

plt.figure()
plt.plot(p, amdahl, marker='o')
plt.xlabel("Number of processors (p)")
plt.ylabel("Speedup S(p)")
plt.title("Amdahl's Law – Strong Scaling")
plt.grid(True)
plt.savefig("amdahl_scaling.png")

plt.figure()
plt.plot(p, gustafson, marker='o')
plt.xlabel("Number of processors (p)")
plt.ylabel("Speedup S_G(p)")
plt.title("Gustafson's Law – Weak Scaling")
plt.grid(True)
plt.grid(True)
plt.savefig("gustafson_scaling.png")

plt.show()

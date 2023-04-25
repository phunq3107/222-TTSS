# package: networkx

from networkx.generators.random_graphs import erdos_renyi_graph
import random
n = 1000
p = 0.5
g = erdos_renyi_graph(n, p)
# print(g.edges)

data = []
for i in range(n):
    data.append([999999] * n)

for (x, y) in g.edges:
    w  = random.randint(1, 100)
    data[x][y] = data[y][x] = w 

for i in range(n):
    data[i][i] = 0

with open(f"data/data_{n}.txt", "w") as f:
    f.write(str(n))
    f.write("\n")
    for i in range(n):
        for j in range(n):
            f.write(str(data[i][j]))
            f.write(" " if j < n-1 else "\n")
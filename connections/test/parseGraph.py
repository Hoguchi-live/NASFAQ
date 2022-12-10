from pyvis.network import Network
import matplotlib.pyplot as plt
import networkx as nx
import pyvis
import json

MAX_DISPLAY = 3
THRESHOLD = 5

fun = lambda x : 10-x

json_path = "graph.json"

net = Network(height='980px', width='100%', bgcolor='#222222', font_color='white')
net.barnes_hut()

with open(json_path, 'r') as f:
    data = json.load(f)

sources, targets, weights = [], [], []

for parent, element in data.items():
    tmp_targets, tmp_weights = [], []
    for child, weight in element.items():
        if parent == child: pass
        else:
            adj_weight = fun(weight)
            net.add_node(parent, parent, title = parent)
            net.add_node(child, child, title = child)

            tmp_targets.append(child)
            tmp_weights.append( round(adj_weight, 2) )

    s1, s2 = (list(reversed(t)) for t in zip(*sorted(zip(tmp_weights, tmp_targets))))
    for i in range(min(MAX_DISPLAY, len(s2))):
        w = round(s1[i], 2)
        if w >= THRESHOLD:
            net.add_edge(parent, s2[i], value = w, title = s1[i])

neighbor_map = net.get_adj_list()
edges = net.get_edges()
nodes = net.get_nodes()

N_nodes = len(nodes)
N_edges = len(edges)

weights=[[] for i in range(N_nodes)]

#Associating weights to neighbors
for i in range(N_nodes): #Loop through nodes
    for neighbor in neighbor_map[nodes[i]]: #and neighbors
        for j in range(N_edges): #associate weights to the edge between node and neighbor
            if (edges[j]['from']==nodes[i] and edges[j]['to']==neighbor) or \
               (edges[j]['from']==neighbor and edges[j]['to']==nodes[i]):
                weights[i].append(edges[j]['value'])

for node,i in zip(net.nodes,range(N_nodes)):

    node['value']=len(neighbor_map[node['id']])
    node['weight']=[str(weights[i][k]) for k in range(len(weights[i]))]
    list_neighbor=list(neighbor_map[node['id']])

    #Sort by score
    w_list = [node['weight'][k] for k in range(node['value'])]
    n_list = [list_neighbor[k] for k in range(node['value'])]

    try:
        s_weights, s_neighbors = (list(t) for t in zip(*sorted(zip(w_list, n_list))))

        #Concatenating neighbors and weights
        hover_str=[s_neighbors[k]+' '+ s_weights[k] for k in range(node['value'])]

        #Setting up node title for hovering
        node['title']+=' Neighbors:<br>'+'<br>'.join(hover_str)
    except ValueError:
        pass

net.toggle_physics(True)
net.show("graph.html")

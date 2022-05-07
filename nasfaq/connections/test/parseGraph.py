from pyvis.network import Network
import matplotlib.pyplot as plt
import networkx as nx
import pyvis
import json

THRESHOLD = 10

def make_net(N, json_path):
    with open(json_path, 'r') as f:
        data = json.load(f)

    weighted_edges = []
    for parent, element in data.items():
        for child, weight in element.items():
            if parent == child: pass
            elif weight > THRESHOLD: pass
            else:
                weighted_edges.append((parent, child, round(weight, 1)))

    # Add edges
    N.add_weighted_edges_from(weighted_edges)

def nudge(pos, x_shift, y_shift):
    return {n:(x + x_shift, y + y_shift) for n,(x,y) in pos.items()}

net = nx.Graph()
make_net(net, "graph.json")

pos = nx.circular_layout(net)

nx.draw_networkx(
    net, pos, edge_color='black', width=1, linewidths=1,
    node_size=100, node_color='pink', alpha=0.9,
    with_labels = False)
    #labels={node: node for node in net.nodes()}

edge_labels = dict([((n1, n2), net[n1][n2]['weight'])
                    for n1, n2 in net.edges])

#nx.draw_networkx_edge_labels(
#    net, pos,
#    edge_labels=edge_labels,
#    font_color='red'
#)


pretty_net = Network(height='750px', width='100%', bgcolor='#222222', font_color='white')
pretty_net.barnes_hut()
pretty_net.from_nx(net)
pretty_net.show_buttons(filter_=['physics'])




pretty_net.show("graph.html")


#plt.axis('off')
#plt.show()

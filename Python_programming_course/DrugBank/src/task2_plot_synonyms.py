"""
Task 2.2 - generate a graph of synonyms using NetworkX.
"""

import pandas as pd
import networkx as nx
import matplotlib.pyplot as plt

def plot_synonyms_graph(drugbank_id, df_path="drug_synonyms.csv", output_path="synonyms_graph.png"):
    df = pd.read_csv(df_path)
    row = df[df["DrugBank ID"] == drugbank_id]

    if row.empty:
        print(f"DrugBank ID {drugbank_id} not found.")
        return
    
    synonyms = row.iloc[0]["Synonyms"]
    if not synonyms or pd.isna(synonyms):
        print(f"No synonyms found for {drugbank_id}.")
        return
    
    synonyms_list = synonyms.split("; ")
    
    G = nx.Graph()
    G.add_node(drugbank_id, color="darksalmon")
    for synonym in synonyms_list:
        G.add_node(synonym, color="lightblue")
        G.add_edge(drugbank_id, synonym)
    
    plt.figure(figsize=(8, 6))
    pos = nx.spring_layout(G)
    colors = [G.nodes[node]["color"] for node in G.nodes]
    nx.draw(G, pos, with_labels=True, node_color=colors, edge_color="gray", \
            font_size=10, node_size=1000, font_weight='bold')
    plt.title(f"Synonyms Graph for {drugbank_id}")

    plt.savefig(output_path, format="png", dpi=300)
    print(f"Task 2.2 completed: graph saved to {output_path}.")
    
    plt.show()

# Run if executed directly (Example usage)
if __name__ == "__main__":
    plot_synonyms_graph("DB00001", "drug_synonyms.csv", "synonyms_DB00001.png")
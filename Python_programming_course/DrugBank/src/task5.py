"""
Task 5 - create a data frame containing information about all pathways
and the drugs that interact with them; additionally present the results
in a graph form.
"""

import pandas as pd
import networkx as nx
import matplotlib.pyplot as plt
from src.data_loader import root, namespace  # Import preloaded XML data

def process_task():
    data = []

    for drug in root.findall("drugbank:drug", namespace):
        drug_name = drug.find("drugbank:name", namespace).text \
            if drug.find("drugbank:name", namespace) is not None else None
        
        for pathway in drug.findall("drugbank:pathways/drugbank:pathway", namespace):
            pathway_id = pathway.find("drugbank:smpdb-id", namespace).text \
                if pathway.find("drugbank:smpdb-id", namespace) is not None else None
            pathway_name = pathway.find("drugbank:name", namespace).text \
                if pathway.find("drugbank:name", namespace) is not None else None
        
            data.append({
                "Drug Name": drug_name,
                "Pathway ID": pathway_id,
                "Pathway Name": pathway_name
            })

    df = pd.DataFrame(data)

    df.to_csv("drug_pathway_interactions.csv", index=False)

    G = nx.Graph()

    drugs = set(df["Drug Name"].unique())
    pathways = set(df["Pathway Name"].unique())

    # Add nodes
    G.add_nodes_from(drugs, bipartite=0)
    G.add_nodes_from(pathways, bipartite=1)

    # Add edges (drug-pathway interactions)
    for _, row in df.iterrows():
        G.add_edge(row["Drug Name"], row["Pathway Name"])

    # Create a properly aligned bipartite layout
    pos = {}
    pos.update((node, (1, i)) for i, node in enumerate(drugs))  # Left side
    pos.update((node, (2, i)) for i, node in enumerate(pathways))  # Right side

    # Draw the bipartite graph
    plt.figure(figsize=(12, 8))
    nx.draw(G, pos, with_labels=True, node_size=500, node_color='lightskyblue', \
            font_size=8, edge_color="gray", font_weight='bold')
    plt.title("Drug-Pathway Bipartite Graph")
    plt.savefig("drug_pathway_graph.png", format="png", dpi=300)
    plt.show()

    print("Task 5 completed: graph saved as drug_pathway_graph.png.")

# Run function if executed directly
if __name__ == "__main__":
    process_task()
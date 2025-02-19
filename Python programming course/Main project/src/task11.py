"""
Task 11 - create a graph of information about drugs that interact with the C1QA gene
and pharmaceutical products containing those drugs.
"""

import pandas as pd
import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from src.data_loader import root, namespace  # Import preloaded XML data

def process_task():
    data = []
    selected_gene = "C1QA"

    for drug in root.findall("drugbank:drug", namespace):
        drug_name = drug.find("drugbank:name", namespace)
        if drug_name is not None:
            drug_name = drug_name.text
        
        for target in drug.findall("drugbank:targets/drugbank:target", namespace):
            polypeptide = target.find("drugbank:polypeptide", namespace)
            
            if polypeptide is not None:
                gene_name = polypeptide.find("drugbank:gene-name", namespace)
                
                if gene_name is not None and gene_name.text == selected_gene:
                    
                    # Find pharmaceutical products associated with this drug
                    products = []
                    for product in drug.findall("drugbank:products/drugbank:product", namespace):
                        product_name = product.find("drugbank:name", namespace)
                        if product_name is not None:
                            products.append(product_name.text)
                    
                    data.append({
                        "Gene Name": gene_name.text,
                        "Drug Name": drug_name,
                        "Pharmaceutical Products": products
                    })

    df = pd.DataFrame(data)

    G = nx.Graph()

    for _, row in df.iterrows():
        G.add_node(row["Gene Name"], type='gene', color='coral', size=1000)
        G.add_node(row["Drug Name"], type='drug', color='skyblue', size=800)
        G.add_edge(row["Gene Name"], row["Drug Name"])  # Connect gene to drug
        
        for product in row["Pharmaceutical Products"]:
            G.add_node(product, type='product', color='darkseagreen', size=600)
            G.add_edge(row["Drug Name"], product)  # Connect drug to pharmaceutical product

    color_map = [nx.get_node_attributes(G, 'color')[node] for node in G.nodes]
    size_map = [nx.get_node_attributes(G, 'size')[node] for node in G.nodes]

    plt.figure(figsize=(12, 8))
    pos = nx.spring_layout(G)
    nx.draw(G, pos, with_labels=True, node_size=size_map, font_size=8, edge_color="gray", node_color=color_map)

    # Add legend
    legend_patches = [
        mpatches.Patch(color='coral', label='Gene (C1QA)'),
        mpatches.Patch(color='skyblue', label='Interacting Drug'),
        mpatches.Patch(color='darkseagreen', label='Pharmaceutical Product')
    ]
    plt.legend(handles=legend_patches, loc='best')

    plt.title("Interactions of Gene C1QA with Drugs and Pharmaceutical Products")
    plt.savefig("gene_C1QA_drug_product_graph.png", format="png", dpi=300)
    plt.show()

    print("Task 11 completed: graph saved as gene_C1QA_drug_product_graph.png.")

# Run function if executed directly
if __name__ == "__main__":
    process_task()
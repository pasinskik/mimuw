"""
Task 6 - for each drug in the database, determine the number of pathways it interacts with
and present the results as a histogram.
"""

import pandas as pd
import matplotlib.pyplot as plt
from src.data_loader import root, namespace  # Import preloaded XML data

def process_task():
    data = []

    for drug in root.findall("drugbank:drug", namespace):
        drug_name = drug.find("drugbank:name", namespace).text \
            if drug.find("drugbank:name", namespace) is not None else None
        
        pathway_count = len(drug.findall("drugbank:pathways/drugbank:pathway", namespace))
        
        if drug_name and pathway_count > 0:
            data.append({
                "Drug Name": drug_name,
                "Pathway Count": pathway_count
            })

    df = pd.DataFrame(data)

    df.to_csv("drug_pathway_counts.csv", index=False)

    # Plot histogram
    plt.figure(figsize=(12, 6))
    plt.bar(df["Drug Name"], df["Pathway Count"], color="slateblue")
    plt.xlabel("Drug Name")
    plt.ylabel("Number of Pathways")
    plt.title("Number of Pathways Associated with Each Drug")
    plt.xticks(rotation=90, fontsize=8)
    plt.tight_layout()
    plt.savefig("drug_pathway_histogram.png", format="png", dpi=300)
    plt.show()

    print("Task 6 completed: histogram saved as drug_pathway_histogram.png.")

# Run function if executed directly
if __name__ == "__main__":
    process_task()
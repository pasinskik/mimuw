"""
Task 4 - create a data frame containing information about all types of pathways (e.g., signaling, metabolic, etc.)
that interact with any drug; additionally provide the total number of unique pathways.
"""

import pandas as pd
from src.data_loader import root, namespace  # Import preloaded XML data

def process_task():

    data = []

    for drug in root.findall("drugbank:drug", namespace):

        for pathway in drug.findall("drugbank:pathways/drugbank:pathway", namespace):
            pathway_id = pathway.find("drugbank:smpdb-id", namespace).text \
                if pathway.find("drugbank:smpdb-id", namespace) is not None else None
            pathway_name = pathway.find("drugbank:name", namespace).text \
                if pathway.find("drugbank:name", namespace) is not None else None
            pathway_type = pathway.find("drugbank:category", namespace).text \
                if pathway.find("drugbank:category", namespace) is not None else None
            
            data.append({
                "Pathway ID": pathway_id,
                "Pathway Name": pathway_name,
                "Pathway Type": pathway_type
            })

    df = pd.DataFrame(data)

    df.to_csv("drug_pathways.csv", index=False)
    print("Task 4 completed: drug_pathways.csv saved.")

    unique_pathways_count = df["Pathway ID"].nunique()
    print(f"Total number of unique pathways: {unique_pathways_count}")

# Run function if executed directly
if __name__ == "__main__":
    process_task()
"""
Task 10 - create a data frame that keeps information about potential drug-drug interactions.
"""

import pandas as pd
from src.data_loader import root, namespace  # Import preloaded XML data

def process_task():
    data = []

    for drug in root.findall("drugbank:drug", namespace):
        drug_name = drug.find("drugbank:name", namespace).text \
            if drug.find("drugbank:name", namespace) is not None else None
        
        for interaction in drug.findall("drugbank:drug-interactions/drugbank:drug-interaction", namespace):
            interacting_drug = interaction.find("drugbank:name", namespace)
            description = interaction.find("drugbank:description", namespace)
            
            if interacting_drug is not None:
                interacting_drug_name = interacting_drug.text
                interaction_description = description.text \
                    if description is not None else "No description available"
                
                data.append({
                    "Drug Name": drug_name,
                    "Interacting Drug Name": interacting_drug_name,
                    "Interaction Description": interaction_description
                })

    df = pd.DataFrame(data)

    df.to_csv("drug_interactions.csv", index=False)

    print("Task 10 completed: drug_interactions.csv saved.")

# Run function if executed directly
if __name__ == "__main__":
    process_task()

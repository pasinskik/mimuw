"""
Task 1 - create a data frame that for each drug will keep: its ID, name, type, description,
state (dosage form), indications (when it should be used), mechanism of action and food interactions.
"""

import pandas as pd
from src.data_loader import root, namespace  # Import preloaded XML data

# Remove newlines and extra spaces from text
def clean_text(text):
    return " ".join(text.split()) if text else None

def process_task():
    data = []
    
    for drug in root.findall("drugbank:drug", namespace):
        drug_id = drug.find("drugbank:drugbank-id[@primary='true']", namespace).text \
            if drug.find("drugbank:drugbank-id[@primary='true']", namespace) is not None else None
        name = drug.find("drugbank:name", namespace).text \
            if drug.find("drugbank:name", namespace) is not None else None
        drug_type = drug.get("type")
        description = drug.find("drugbank:description", namespace).text \
            if drug.find("drugbank:description", namespace) is not None else None
        state = drug.find("drugbank:state", namespace).text \
            if drug.find("drugbank:state", namespace) is not None else None
        indication = drug.find("drugbank:indication", namespace).text \
            if drug.find("drugbank:indication", namespace) is not None else None
        mechanism_of_action = drug.find("drugbank:mechanism-of-action", namespace).text \
            if drug.find("drugbank:mechanism-of-action", namespace) is not None else None

        food_interactions = drug.find("drugbank:food-interactions", namespace)
        if food_interactions is not None:
            food_interaction_list = [interaction.text for interaction in food_interactions.findall("drugbank:food-interaction", namespace)]
            food_interactions = "; ".join(food_interaction_list)
        else:
            food_interactions = None

        data.append({
            "DrugBank ID": drug_id,
            "Name": name,
            "Type": drug_type,
            "Description": clean_text(description),
            "State": clean_text(state),
            "Indication": clean_text(indication),
            "Mechanism of Action": clean_text(mechanism_of_action),
            "Food Interactions": clean_text(food_interactions)
        })
    
    df = pd.DataFrame(data)
    df.to_csv("drug_data_summary.csv", index=False)
    print("Task 1 completed: drug_data_summary.csv saved.")

# Run function if executed directly
if __name__ == "__main__":
    process_task()
"""
Task 13 - Generate a simulated DrugBank database with 200 drugs (100 new drugs
with randomly assigned properties).
"""

import xml.etree.ElementTree as ET
import random
from copy import deepcopy
from src.data_loader import root, namespace  # Import preloaded XML data

ET.register_namespace("", "http://www.drugbank.ca")  # Prevents ns0 prefix

def process_task():
    original_drugs = root.findall("drugbank:drug", namespace)

    if len(original_drugs) == 0:
        print("No drugs found in the original XML data.")
        return

    # Create a new root element with the same tag and attributes as the original root
    new_root = ET.Element(root.tag, root.attrib)

    # Append all original drugs to the new root
    for drug in original_drugs:
        new_root.append(deepcopy(drug))

    # Use the first original drug as an exemplar to determine structure and order of child elements
    exemplar = original_drugs[0]

    # Generate 100 synthetic drugs
    for i in range(1, 101):
        new_drug = ET.Element(exemplar.tag, exemplar.attrib)
        # Iterate over each child element (in order) from the exemplar
        for child in exemplar:
            tag = child.tag
            # Special handling for drugbank-id elements
            if tag.endswith("drugbank-id"):
                # If this element has primary="true", replace its text with the new sequential ID
                if child.get("primary") == "true":
                    new_id = ET.Element(tag, {"primary": "true"})
                    new_id.text = f"DB{100000 + i}"
                    new_drug.append(new_id)
                else:
                    # For non-primary drugbank-id elements, pick a random one from all original drugs
                    pool = []
                    for drug in original_drugs:
                        for elem in drug.findall(tag):
                            # Skip the primary ones
                            if elem.get("primary") != "true":
                                pool.append(elem)
                    if pool:
                        chosen = deepcopy(random.choice(pool))
                        new_drug.append(chosen)
            else:
                # For any other tag, gather all occurrences of that element from all original drugs
                pool = []
                for drug in original_drugs:
                    for elem in drug.findall(tag):
                        pool.append(elem)
                if pool:
                    chosen = deepcopy(random.choice(pool))
                    new_drug.append(chosen)
        new_root.append(new_drug)

    # Format the XML with indentation for readability
    ET.indent(new_root, space="  ")

    output_file = "drugbank_partial_and_generated.xml"
    tree = ET.ElementTree(new_root)
    tree.write(output_file, encoding="utf-8", xml_declaration=True)

    print(f"Task 13 completed: database with {len(new_root)} drugs saved as {output_file}.")
    print("In order to analyze the newly created file, simply update the XML_PATH in data_loader.py to drugbank_partial_and_generated.xml.")

# Run function if executed directly
if __name__ == "__main__":
    process_task()

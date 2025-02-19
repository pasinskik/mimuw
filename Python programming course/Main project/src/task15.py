"""
Task 15 - start a FastAPI server to return the number of interaction pathways for a given drug.

Usage:
1. If using students, create a virtual environment:
    python3 -m venv venv
    source venv/bin/activate
    pip install fastapi uvicorn

2. Start the server:
    python3 main.py --task 15

3. Make a request (example using curl):
    curl -X 'POST' 'http://127.0.0.1:8000/get-pathway-count/' \
        -H 'Content-Type: application/json' \
        -d '{"drug_id": "DB00001"}'
"""

from fastapi import FastAPI
from pydantic import BaseModel
import uvicorn
import xml.etree.ElementTree as ET
from src.data_loader import root, namespace  # Import preloaded XML data

# Create a FastAPI instance
app = FastAPI()

# Define a request model for receiving the DrugBank ID
class DrugRequest(BaseModel):
    drug_id: str

# Function to count pathways for a given drug
def get_pathway_count(drug_id: str) -> int:
    drug = root.find(f"drugbank:drug[drugbank:drugbank-id='{drug_id}']", namespace)
    if drug is not None:
        return len(drug.findall("drugbank:pathways/drugbank:pathway", namespace))
    return 0

# POST endpoint to receive a DrugBank ID and return the pathway count
@app.post("/get-pathway-count/")
async def get_pathways(request: DrugRequest):
    pathway_count = get_pathway_count(request.drug_id)
    return {"drug_id": request.drug_id, "pathway_count": pathway_count}

# Function to start FastAPI server
def process_task():
    print("Starting FastAPI server on http://127.0.0.1:8000")
    uvicorn.run(app, host="127.0.0.1", port=8000)

# Run function if executed directly
if __name__ == "__main__":
    process_task()

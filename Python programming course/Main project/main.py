"""
Main script to run specific DrugBank tasks.

Usage:
    python main.py --task <task_number>

Examples:
    python main.py --task 1      # Run task1.py
    python main.py --task 2.1    # Run task2_extract_synonyms.py
    python main.py --task 2.2    # Run task2_plot_synonyms.py
"""

import argparse
import importlib

def main():
    parser = argparse.ArgumentParser(description="Run a specific DrugBank task.")
    parser.add_argument("--task", type=str, required=True, help="Task number to run (e.g., 1, 2.1, 2.2, 3, ...)")
    args = parser.parse_args()
    
    task_mapping = {
        "1": "task1",
        "2.1": "task2_extract_synonyms",
        "2.2": "task2_plot_synonyms",
        "3": "task3",
        "4": "task4",
        "5": "task5",
        "6": "task6",
        "7": "task7",
        "8": "task8",
        "9": "task9",
        "10": "task10",
        "11": "task11",
        "12": "task12",
        "13": "task13",
        "14": "task14",
        "15": "task15"
    }
    
    if args.task not in task_mapping:
        print(f"Error: Task {args.task} not found.")
        return
    
    module_name = f"src.{task_mapping[args.task]}"
    
    try:
        task_module = importlib.import_module(module_name)
        print(f"Running Task {args.task}...")
        
        if args.task == "2.2":
            drug_id = input("Enter DrugBank ID for synonym graph: ")
            task_module.plot_synonyms_graph(drug_id)
        elif hasattr(task_module, "process_task"):            
            # Special case for Task 15 (FastAPI server)
            if args.task == "15":
                print("FastAPI server is running on http://127.0.0.1:8000")
                print("To test, run a curl command in a new terminal, for example:")
                print("   curl -X 'POST' 'http://127.0.0.1:8000/get-pathway-count/' \\")
                print("        -H 'Content-Type: application/json' \\")
                print("        -d '{\"drug_id\": \"DB00001\"}'")
                print("Press Ctrl+C to stop the server.")

            task_module.process_task()
            
        elif hasattr(task_module, "extract_synonyms"):
            task_module.extract_synonyms()
        else:
            print(f"Task {args.task} has no callable function.")
    except ModuleNotFoundError:
        print(f"Error: Module {module_name} not found.")
    
if __name__ == "__main__":
    main()

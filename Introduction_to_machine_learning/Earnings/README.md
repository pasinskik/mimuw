# Wage Structure Analysis – Machine Learning Project

This project explores the **2010 wage structure dataset** using a range of statistical and machine learning methods.  
It includes data exploration, clustering, classification, and regression - culminating in salary prediction using ensemble models.

---

## Overview

We aim to uncover patterns and predictive factors in employee earnings, with tasks including:

- **Data inspection**: summary statistics, distributions, correlations  
- **Clustering**: grouping similar employees using K-Means and silhouette scores  
- **Classification**: predicting whether an employee has a higher-education degree  
- **Regression**: modeling base salary using linear regression and tree-based ensembles  

Each task is performed with a focus on clear variable handling, log-transformation where needed, and proper train/test validation.

---

## Tasks & Methods

### Task 1 - Data Inspection  
- Summary statistics and visualizations  
- Log-transforms for skewed earnings variables  
- Correlation heatmaps and scatterplots

### Task 2 - Clustering  
- K-Means on preprocessed numeric features  
- Feature scaling and log-transform applied  
- Optimal number of clusters selected via silhouette analysis  
- Visualized using PCA space

### Task 3 - Classification  
- Goal: predict `education <= 2` (higher education)  
- Compared Logistic Regression, Random Forest, Gradient Boosting  
- 5-fold cross-validation for accuracy and ROC AUC  
- Feature importance analysis on best model

### Task 4 - Regression  
- Target: base salary (log-transformed for stability)  
- Redundant features dropped (e.g., `age`, `duration_overtime`)  
- Final model: **Random Forest**  
- Evaluated with RMSE and R² on test set  
- Top feature importances identified (`other`, `duration_nominal`, etc.)  

---

## Results

| Task            | Method                  | Key Metric (Test)         |
|-----------------|-------------------------|---------------------------|
| Classification  | Gradient Boosting       | ROC AUC ≈ 0.95            |
| Regression      | Random Forest (log base)| RMSE ≈ 8,687 PLN, R² ≈ 0.80 |

Bootstrap resampling was used to estimate uncertainty in salary predictions.

---

## How to Run

1. Create and activate a virtual environment:
   ```bash
   python3 -m venv venv
   source venv/bin/activate
   ```

2. Install requirements:
   ```bash
     pip install -r requirements.txt
     pip install ipykernel
   ```

3. Register the kernel:
   ```bash
   python -m ipykernel install --user --name=wum_env --display-name "Python (wum_env)"
   ```

4. Launch the notebook:
   ```bash
     jupyter notebook
   ```

5. Select kernel:
     Kernel -> Change Kernel -> Python (wum_env)

6. Open `notebook.ipynb` and select Kernel -> Restart & Run All
# Variable Prediction – Machine Learning Project

This project tackles the **prediction of both a binary class and a continuous output** variable from a synthetic dataset of 2,000 samples and 400 real-valued input features.  
We implement and compare linear and non-linear models for both classification and regression, emphasizing feature selection, validation, and performance tuning.

---

## Overview

We work on two machine learning tasks using the same input space:

- **Classification**: predict a binary `Class` variable (0/1)  
- **Regression**: estimate a continuous `Output` variable

The data is standard-scaled, synthetic, and designed to test both baseline and advanced methods.  
Our goals are to surpass **0.80 classification accuracy** and **0.50 R²** in regression.

---

## Tasks & Methods

### Task 1 - Data Preparation  
- Loaded 2,000 samples with 400 numerical features + 2 targets  
- Verified shapes, missing values, and class balance  
- Data already standardized – no preprocessing required  

### Task 2 - Classification  
- Goal: predict binary target `Class`  
- Compared models:
  - **Logistic Regression** (baseline)
  - **Random Forest**
  - **Gradient Boosting**  
- 5-fold cross-validation for evaluation (accuracy)  
- Used `SelectKBest` with mutual information to reduce dimensionality  
- Final model: **Gradient Boosting Classifier**

### Task 3 - Regression  
- Goal: predict continuous target `Output`  
- Tried:
  - **Linear Regression** (baseline)
  - **Lasso Regression** with feature selection
  - **Random Forest Regressor**  
- Feature selection via `SelectKBest` (mutual information) and Lasso  
- Evaluated with R² and RMSE on validation set  
- Final model: **Random Forest Regressor**

---

## Results

| Task           | Method                  | Metric (CV/Test)         |
|----------------|-------------------------|---------------------------|
| Classification | Gradient Boosting       | Accuracy ≈ 0.83           |
| Regression     | Random Forest Regressor | R² ≈ 0.57, RMSE ≈ 0.28    |

Both tasks surpassed the target thresholds using ensemble models with light feature selection.

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

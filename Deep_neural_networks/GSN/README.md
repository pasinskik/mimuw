# Multitask Deep Neural Network for Shape Counting (GSN)

This project implements a multitask convolutional neural network for a synthetic shape-counting dataset.  
The model jointly learns:

- a **135-way classification task** (shape pair + count split), and  
- a **6-dimensional regression task** (counts of each shape type).

---

## 1. Requirements

- Python 3.9+
- PyTorch
- scikit-learn
- NumPy
- pandas
- matplotlib
- torchvision

You can install the dependencies with:

---

```bash
pip install -r requirements.txt
```

## 2. Dataset

The code expects the dataset in the following structure:

```bash
data/
  labels.csv
  images/
    <image files>.png
```

---

## 3. Running the training pipeline

For the sake of grading or getting familiar with the model, you can run the Python function 
```python
run_three_experiments()
```
to be able to see the results of three different learning modes (classification only, regression only, multitasking).

However, keep in mind that the code <b>was designed to be run as a single model with many customizable parameters.</b>
Single experiment example:
```bash
python train_multitask.py --mode joint --lambda_reg 1.0 --epochs 100 --patience 10 --device cuda
```

This will:
- load the dataset,
- perform a stratified 9000/1000 train/validation split,
- train the model with early stopping,
- save the best checkpoint to `best_model_<run_name>.pt`,
- and save learning curves (`loss_curves_<run_name>.png`, `val_acc_<run_name>.png`, `val_rmse_<run_name>.png`).

### 3.1 Reproducing three configurations used in the report

In order to reproduce configurations used in the report, please make sure that
```python
if __name__ == "__main__":
    run_three_experiments()
```
and then simply run:

```bash
python train_multitask.py
```

This sequentially runs:
- cls (classification-only)
- joint (multitask, λ = 1.0)
- reg (regression-only)

Each experiment saves its own model and plots with corresponding name suffixes.

---

## 4. Google Colab

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](
https://github.com/pasinskik/mimuw/blob/main/Deep_neural_networks/GSN/notebooks/gsn_multitask_colab.ipynb
)


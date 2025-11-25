#!/usr/bin/env python3
"""
Deep Neural Networks Multitask Counting Project.

This script implements:
    - A custom PyTorch Dataset for loading images + labels
    - Label preprocessing and augmentation
    - Classification target encoding for 135 classes
    - Utilities for flipping/rotating the label counts

The goal of the dataset is to return:
    (image_tensor, class_id, counts_tensor)
for each example.
"""
import os
import argparse
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import random
import torchvision.transforms.functional as TF
from typing import Counter, Tuple
from sklearn.model_selection import train_test_split
from sklearn.metrics import f1_score

import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.utils.data import Dataset, DataLoader, Subset

# ----- Set all random seeds for reproducibility. ----- #
# Ensures that data shuffling, augmentations, and weight initialization
# behave consistently across runs.
torch.manual_seed(1)
random.seed(1)
np.random.seed(1)

# ----- PAIR_LIST defines all 15 unordered pairs of shape types. ----- #
# There are 6 shape types indexed as:
#   0 = squares
#   1 = circles
#   2 = up
#   3 = right
#   4 = down
#   5 = left
# Each image always contains exactly TWO non-zero shape types.
# (6 * 5) / 2 = 15 possible pairs, order doesn't matter.
PAIR_LIST = [
    (0,1),(0,2),(0,3),(0,4),(0,5),
    (1,2),(1,3),(1,4),(1,5),
    (2,3),(2,4),(2,5),
    (3,4),(3,5),
    (4,5),
]

# ----- DATASET DEFINITION ----- #
class GSNCountingDataset(Dataset):
    """
    It returns:
        - image_tensor : shape (1, 28, 28), float32
        - class_id     : integer in [0, 134] for classification
        - counts       : tensor of shape (6,) with count of each shape type.

    The dataset reads its metadata from labels.csv and loads images
    from a specified folder.
    """

    def __init__(self, root_dir, csv_path, augmentations=True):
        """
        Parameters:
        - root_dir:     folder containing all images (PNG files)
        - csv_path:     path to the labels.csv file
        - augmentations: if True, apply data augmentation (used for training).
        """
        self.root_dir = root_dir
        self.df = pd.read_csv(csv_path)
        self.augmentations = augmentations

    def __len__(self):
        """Return the total number of samples."""
        return len(self.df)

    def compute_class_id(self, row) -> int:
        """
        Convert counts of 6 shape types -> class ID in 0..134.
        Each image has exactly two nonzero counts, each in 1..9.
        The mapping is as follows:
            - There are 15 possible pairs of shapes (order doesn't matter).
            - For each pair, there are 9 possible splits (0..8 for the first
                shape, the rest for the second shape).
        Total: 15 * 9 = 135 classes.
        0: (squares, circles), split=0
        1: (squares, circles), split=2
        ...
        8: (squares, circles), split=8
        9: (squares, up), split=0
        ...
        134: (left, down), split=8
        135 classes total.
        """
        counts = [
            row["squares"],
            row["circles"],
            row["up"],
            row["right"],
            row["down"],
            row["left"],
        ]

        # Indexes of the two present shapes.
        active = [i for i, c in enumerate(counts) if c > 0]

        if len(active) != 2:
            raise ValueError(f"Expected exactly 2 active shapes, got {len(active)} in row {row}.")

        i, j = active   # i < j because enumeration is sorted

        pair_index = PAIR_LIST.index((i, j))

        # The split index (0..8).
        # Split is "how many the FIRST shape has".
        split_index = counts[i] - 1     # because count_i is 1..9

        class_id = pair_index * 9 + split_index
        return class_id
        
    def rotate_orientation_labels(self, counts, k):
        """
        Helper for rotation augmentation.
        Rotate orientation counts by k * 90 degrees clockwise.
        counts = tensor([squares, circles, up, right, down, left]).
        """
        squares, circles, up, right, down, left = counts

        for _ in range(k):
            up, right, down, left = left, up, right, down  # rotate clockwise

        return torch.tensor([squares, circles, up, right, down, left], dtype=torch.float32)

    def __getitem__(self, idx):
        row = self.df.iloc[idx]

        # Load image.
        img_path = os.path.join(self.root_dir, row["name"])
        image = torch.tensor(plt.imread(img_path), dtype=torch.float32)

        # Images in the dataset are RGBA (4 channels), not grayscale.
        # Convert to grayscale by averaging channels if needed.
        if image.ndim == 3 and image.shape[2] == 4:
            image = image.mean(dim=2)   # to grayscale

        image = image.unsqueeze(0)      # (H,W) -> (1, H, W)

        # Load count labels for all 6 shape types.
        counts = torch.tensor([
            row["squares"], row["circles"],
            row["up"], row["right"], row["down"], row["left"]
        ], dtype=torch.float32)

        # Compute class ID for classification task.
        class_id = torch.tensor(self.compute_class_id(row), dtype=torch.long)

        # Data augmentations (used only during training).
        # Apply with 50% probability each.
        if self.augmentations:
            # 1) Rotation by multiples of 90 degrees.
            # Note: we need to adjust the orientation counts accordingly.
            if random.random() < 0.5:
                k = random.choice([1, 2, 3])   # 90, 180, 270
                image = torch.rot90(image, k, dims=[1,2])
                counts = self.rotate_orientation_labels(counts, k)

            # 2) Adjust contrast and brightness.
            if random.random() < 0.5:
                image = TF.adjust_brightness(image, 1 + random.uniform(-0.2, 0.2))
                image = TF.adjust_contrast(image, 1 + random.uniform(-0.2, 0.2))

            # 3) Add Gaussian noise.
            if random.random() < 0.5:
                image = image + 0.05 * torch.randn_like(image)

            # Clip to [0, 1] range after augmentations.
            image = torch.clamp(image, 0, 1)

        return image, class_id, counts


# ----- MODEL DEFINITION ----- #
class MultiTaskNet(nn.Module):
    """
    Multitask CNN consisting of:
        - A REQUIRED backbone (exact architecture defined by the assignment)
        - A classification head (135 classes)
        - A regression head (predict 6 shape counts).

    The model outputs:
        log_probs : log-probabilities over 135 classes  (for NLLLoss)
        counts    : 6 continuous regression predictions.
    """
    def __init__(self):
        super().__init__()

        # Backbone has been mandated by the assignment.
        # This is a deep convolutional feature extractor that:
        #   - takes a 1×28×28 grayscale image
        #   - applies several Conv+ReLU layers
        #   - flattens the output
        #   - maps it to a 256-dimensional feature vector.
        self.backbone = nn.Sequential(
            nn.Conv2d(1, 8, 3, stride=1, padding=1), nn.ReLU(),
            nn.Conv2d(8, 16, 3, stride=1, padding=1), nn.ReLU(),
            nn.Conv2d(16, 32, 3, stride=1, padding=1), nn.ReLU(),
            nn.Conv2d(32, 64, 3, stride=1, padding=1), nn.ReLU(),

            # Output shape here is (64, 28, 28).
            nn.Flatten(start_dim=1),

            # Fully connected to 256-dim feature vector.
            nn.Linear(64 * 28 * 28, 256), nn.ReLU()
        )

        # ----- Classification Head -----
        # Maps the shared 256-D feature representation to 135 classes.
        # We will apply log_softmax in the forward pass because the
        # assignment requires using NLLLoss.
        self.head_cls = nn.Linear(256, 135)

        # ----- Regression Head -----
        # Maps the shared 256-D feature representation to 6 counts.
        self.head_cnt = nn.Linear(256, 6)

    # Forward pass.
    def forward(self, x):
        """
        Returns:
            log_probs : log-probabilities (N, 135)
            counts    : regression predictions (N, 6).
        """

        # Extract shared visual features from the backbone.
        features = self.backbone(x)

        # Classification output -> convert logits -> log-probabilities.
        logits = self.head_cls(features)
        log_probs = F.log_softmax(logits, dim=1)

        # Regression output.
        counts = self.head_cnt(features)

        return log_probs, counts


# ----- TRAINING FUNCTION ----- #
def train_one_epoch(model, loader, optimizer, device, lambda_reg, mode="joint"):
    """
    Performs one full pass over the training dataset.

    Loss = NLLLoss(classification) + lambda * MSELoss(regression).

    Parameters:
        model        : the neural network
        loader       : DataLoader providing batches of training data
        optimizer    : Adam optimizer
        device       : CPU/GPU
        lambda_reg   : weight for the regression loss component.

    Returns:
        mean training loss for the epoch
    """

    model.train()           # enable dropout/batchnorm if any
    ce = nn.NLLLoss()       # required for log_probs
    mse = nn.MSELoss()

    total_loss = 0

    for images, class_id, counts in loader:
        # Move batch to CPU/GPU
        images = images.to(device)
        class_id = class_id.to(device)
        counts = counts.to(device)

        optimizer.zero_grad()       # clear old gradients

        # Forward pass -> compute predictions
        log_probs, pred_counts = model(images)

        # Two separate loss terms
        loss_cls = ce(log_probs, class_id)      # classification loss
        loss_reg = mse(pred_counts, counts)     # regression loss

        # Combine them according to the selected mode
        if mode == "cls":
            loss = loss_cls                     # classification only
        elif mode == "reg":
            loss = lambda_reg * loss_reg        # regression only
        else:   # "joint"
            loss = loss_cls + lambda_reg * loss_reg
        # Backpropagate errors
        loss.backward()
        optimizer.step()

        # Accumulate weighted batch loss
        total_loss += loss.item() * images.size(0)

    return total_loss / len(loader.dataset)

# ----- VALIDATION FUNCTION ----- #
@torch.no_grad()
def validate(model, loader, device, lambda_reg, mode="joint"):
    """
    Evaluates the model on the validation set.

    Computes:
        - total multitask loss
        - top-1 accuracy
        - macro F1-score
        - per-pair accuracy (classification but ignoring split)
        - overall RMSE, overall MAE
        - per-class RMSE (6 values)
        - per-class MAE (6 values).

    Returns a tuple of all metrics.
    """

    model.eval()            # evaluation mode
    ce = nn.NLLLoss()
    mse = nn.MSELoss()

    total_loss = 0
    correct = 0

    all_preds = []
    all_true = []
    all_pred_counts = []
    all_true_counts = []

    # Iterate over validation batches
    for images, class_id, counts in loader:
        images = images.to(device)
        class_id = class_id.to(device)
        counts = counts.to(device)

        # Forward pass (no gradients)
        log_probs, pred_counts = model(images)

        # Compute losses
        loss_cls = ce(log_probs, class_id)
        loss_reg = mse(pred_counts, counts)

        # Combine them according to the selected mode
        if mode == "cls":
            loss = loss_cls                     # classification only
        elif mode == "reg":
            loss = lambda_reg * loss_reg        # regression only
        else:   # "joint"
            loss = loss_cls + lambda_reg * loss_reg

        total_loss += loss.item() * images.size(0)
        
        # Compute number of correct predictions.
        correct += (log_probs.argmax(dim=1) == class_id).sum().item()

        # Store all predictions and true labels for metrics computation.
        all_preds.append(log_probs.argmax(dim=1).cpu())
        all_true.append(class_id.cpu())
        all_pred_counts.append(pred_counts.cpu())
        all_true_counts.append(counts.cpu())

    # Concatenate all batches.
    all_preds = torch.cat(all_preds)
    all_true = torch.cat(all_true)
    all_pred_counts = torch.cat(all_pred_counts)
    all_true_counts = torch.cat(all_true_counts)

    # ----- Classification metrics -----
    accuracy = correct / len(loader.dataset)
    f1_macro = f1_score(all_true.numpy(), all_preds.numpy(), average="macro")

    # Per-pair accuracy (ignoring split index)
    true_pairs = (all_true // 9).numpy()
    pred_pairs = (all_preds // 9).numpy()
    pair_acc = (true_pairs == pred_pairs).mean()

    # ----- Regression metrics -----
    diff = all_pred_counts - all_true_counts    # (N, 6)

    rmse_overall = torch.sqrt((diff ** 2).mean()).item()
    mae_overall = diff.abs().mean().item()

    # Per-class RMSE and MAE (shape: 6,)
    mse_per_class = (diff ** 2).mean(dim=0)
    rmse_per_class = torch.sqrt(mse_per_class).tolist()

    mae_per_class = diff.abs().mean(dim=0).tolist()

    return (
        total_loss / len(loader.dataset),
        accuracy,
        f1_macro,
        pair_acc,
        rmse_overall,
        mae_overall,
        rmse_per_class,
        mae_per_class,
    )


def data_analysis():
    """
    Function to perform data analysis on the dataset.
    Generates and saves:
        - Distribution of shape pairs (bar plot)
        - Distribution of count splits (histogram)
        - Shape statistics (mean, std) saved to CSV.
    """

    df = pd.read_csv("data/labels.csv")

    # ----- Pair distribution ----- #
    pairs = []
    for _, row in df.iterrows():
        counts = [
            row["squares"], row["circles"],
            row["up"], row["right"], row["down"], row["left"]
        ]
        active = [i for i, c in enumerate(counts) if c > 0]
        pairs.append(tuple(active))

    pair_counts = Counter(tuple(sorted(p)) for p in pairs)

    plt.figure(figsize=(10,4))
    plt.bar(range(15), list(pair_counts.values()))
    plt.xticks(range(15), [str(p) for p in pair_counts.keys()], rotation=45)
    plt.title("Distribution of Shape Pairs")
    plt.tight_layout()
    plt.savefig("eda_pairs.png")
    plt.close()

    # ----- Split distribution ----- #
    splits = []
    for _, row in df.iterrows():
        counts = [
            row["squares"], row["circles"],
            row["up"], row["right"], row["down"], row["left"]
        ]
        active = [i for i, c in enumerate(counts) if c > 0]
        split = counts[active[0]]
        splits.append(split)

    plt.hist(splits, bins=np.arange(0.5,10.5,1))
    plt.title("Distribution of Count Splits (1–9)")
    plt.xlabel("Count")
    plt.ylabel("Frequency")
    plt.savefig("eda_splits.png")
    plt.close()

    # ----- Shape statistics ----- #
    shapes = ["squares","circles","up","right","down","left"]
    stats = df[shapes].describe().loc[["mean","std"]]
    stats.to_csv("eda_shape_stats.csv")
    print(stats)

def run_experiment(
    data_dir: str = "data/images",
    csv_path: str = "data/labels.csv",
    epochs: int = 10,
    lr: float = 1e-3,
    lambda_reg: float = 1.0,
    device_str: str = "cuda",
    patience: int = 10,
    mode: str = "joint",
):
    """
    Core training+evaluation pipeline.

    This can be called:
      - from main() (CLI)
      - from run_three_experiments() (multiple runs)
    """

    # Select device: use GPU if available and requested, otherwise CPU.
    device = torch.device("cuda" if (device_str == "cuda" and torch.cuda.is_available()) else "cpu")

    # Load full dataset (train + validation) from CSV + images.
    # We will later split it into 90% / 10% samples.
    # Use augmentations=True only to access the dataframe for stratification.
    full_ds = GSNCountingDataset(
        root_dir=data_dir,
        csv_path=csv_path,
        augmentations=True,
    )

    # ----- Train/validation split ----- #
    # Requirements:
    #   - 9000 train, 1000 validation
    #   - stratified by 135-way class_id to preserve class balance.
    dataset_size = len(full_ds)
    all_indices = np.arange(dataset_size)

    # Compute class_id for each sample directly from the CSV row.
    labels = []
    for idx in all_indices:
        row = full_ds.df.iloc[idx]
        class_id = full_ds.compute_class_id(row)
        labels.append(class_id)
    labels = np.array(labels)

    # 90% train / 10% validation, stratified by class_id
    train_idx, val_idx = train_test_split(
        all_indices,
        test_size=0.1,
        random_state=42,
        stratify=labels,
    )

    # Create Subset datasets for train and validation.
    # Note: augmentations=True only for training set.
    train_ds = Subset(GSNCountingDataset(data_dir, csv_path, augmentations=True), train_idx)
    val_ds   = Subset(GSNCountingDataset(data_dir, csv_path, augmentations=False), val_idx)

    # Wrap datasets in DataLoaders.
    train_loader = DataLoader(train_ds, batch_size=64, shuffle=True)
    val_loader = DataLoader(val_ds, batch_size=1000, shuffle=False)

    # ----- Model, optimizer, training loop ----- #
    model = MultiTaskNet().to(device)
    optimizer = torch.optim.Adam(model.parameters(), lr=lr)

    # Dictionary to store training history for plotting later.
    history = {
        "epoch": [],
        "train_loss": [],
        "val_loss": [],
        "val_acc": [],
        "val_rmse": [],
    }

    # Early stopping variables.
    best_val_loss = float("inf")
    epochs_no_improve = 0
    best_model_path = f"best_model_{mode}.pt"

    # Training loop.
    for epoch in range(1, epochs + 1):
        train_loss = train_one_epoch(
            model, train_loader, optimizer, device, lambda_reg, mode
        )
        
        (
            val_loss,
            val_acc,
            val_f1,
            val_pair_acc,
            val_rmse,
            val_mae,
            val_rmse_per_class,
            val_mae_per_class,
        ) = validate(model, val_loader, device, lambda_reg, mode)

    # Store metrics for plots later.
        history["epoch"].append(epoch)
        history["train_loss"].append(train_loss)
        history["val_loss"].append(val_loss)
        history["val_acc"].append(val_acc)
        history["val_rmse"].append(val_rmse)

        # Print a concise summary for this epoch.
        print(
            f"Epoch {epoch:02d} | "
            f"train_loss={train_loss:.4f} | "
            f"val_loss={val_loss:.4f} | "
            f"val_acc={val_acc:.4f} | "
            f"val_f1={val_f1:.4f} | "
            f"val_pair_acc={val_pair_acc:.4f} | "
            f"val_rmse={val_rmse:.4f} | "
            f"val_mae={val_mae:.4f}"
        )   


        # Early stopping based on validation loss.
        # If val_loss improves, we:
        #   - reset patience counter
        #   - save the current best model weights.
        # If it does not improve for 'patience' epochs, we stop.
        if val_loss < best_val_loss - 1e-4:  # small delta to ignore noise
            best_val_loss = val_loss
            epochs_no_improve = 0
            torch.save(model.state_dict(), best_model_path)
        else:
            epochs_no_improve += 1
            if epochs_no_improve >= patience:
                print(f"Early stopping triggered at epoch {epoch}")
                break


    # ----- Final evaluation with the best model ----- #
    model.load_state_dict(torch.load(best_model_path))

    (
        val_loss,
        val_acc,
        val_f1,
        val_pair_acc,
        val_rmse,
        val_mae,
        val_rmse_per_class,
        val_mae_per_class,
    ) = validate(model, val_loader, device, lambda_reg, mode)

    print("\n=== Final validation metrics (best model) ===")
    print(f"val_loss      = {val_loss:.4f}")
    print(f"val_acc       = {val_acc:.4f}")
    print(f"val_f1        = {val_f1:.4f}")
    print(f"val_pair_acc  = {val_pair_acc:.4f}")
    print(f"val_rmse      = {val_rmse:.4f}")
    print(f"val_mae       = {val_mae:.4f}")
    print("RMSE per class [squares, circles, up, right, down, left]:")
    print(val_rmse_per_class)
    print("MAE per class  [squares, circles, up, right, down, left]:")
    print(val_mae_per_class)

    # ----- Plot training curves ----- #
    # Plots are saved as PNG files in the current directory.
    epochs = history["epoch"]

    # 1) Training & validation loss.
    plt.figure()
    plt.plot(epochs, history["train_loss"], label="train_loss")
    plt.plot(epochs, history["val_loss"], label="val_loss")
    plt.xlabel("Epoch")
    plt.ylabel("Loss")
    plt.legend()
    plt.title("Training and validation loss")
    plt.tight_layout()
    plt.savefig(f"loss_curves_{mode}.png")
    plt.close()

    # 2) Validation accuracy.
    plt.figure()
    plt.plot(epochs, history["val_acc"], label="val_acc")
    plt.xlabel("Epoch")
    plt.ylabel("Accuracy")
    plt.title("Validation accuracy over epochs")
    plt.tight_layout()
    plt.savefig(f"val_acc_{mode}.png")
    plt.close()

    # 3) Validation RMSE.
    plt.figure()
    plt.plot(epochs, history["val_rmse"], label="val_rmse")
    plt.xlabel("Epoch")
    plt.ylabel("RMSE")
    plt.title("Validation RMSE over epochs")
    plt.tight_layout()
    plt.savefig(f"val_rmse_{mode}.png")
    plt.close()


def run_three_experiments(base_args=None):
    """
    Helper function for the purpose of grading.
    Run three experiment configurations:
      1) Classification-only
      2) Joint multitask
      3) Regression-only

    If base_args is provided (Namespace from argparse), take shared
    hyperparameters (data_dir, csv, epochs, lr, device, patience) from it.
    """
    if base_args is None:
        # default values if called from Python, not CLI
        class Dummy:
            data_dir = "data/images"
            csv = "data/labels.csv"
            epochs = 10
            lr = 1e-3
            device = "cuda"
            patience = 10
        base_args = Dummy()

    configs = [
        {"mode": "cls",   "lambda_reg": 0.0},
        {"mode": "joint", "lambda_reg": 1.0},
        {"mode": "reg",   "lambda_reg": 1.0},
    ]

    for cfg in configs:
        print(f"\n========== Running experiment: {cfg['mode']} ==========")
        run_experiment(
            data_dir=base_args.data_dir,
            csv_path=base_args.csv,
            epochs=base_args.epochs,
            lr=base_args.lr,
            lambda_reg=cfg["lambda_reg"],
            device_str=base_args.device,
            patience=base_args.patience,
            mode=cfg["mode"],
        )

# ----- MAIN TRAINING LOOP ----- #
def main():
    # Parse command-line arguments so we can easily run
    # different experiments:
    #   - different λ (lambda_reg)
    #   - different learning rates
    #   - CPU/GPU choice
    #   - number of epochs / patience.
    parser = argparse.ArgumentParser()
    parser.add_argument("--data_dir", type=str, default="data/images",
                        help="Directory with PNG images")
    parser.add_argument("--csv", type=str, default="data/labels.csv",
                        help="Path to labels.csv file")
    parser.add_argument("--epochs", type=int, default=10,
                        help="Maximum number of training epochs")
    parser.add_argument("--lr", type=float, default=1e-3,
                        help="Learning rate for Adam optimizer")
    parser.add_argument("--lambda_reg", type=float, default=1.0,
                        help="Weight for regression loss component")
    parser.add_argument("--device", type=str, default="cuda",
                        help="'cuda' or 'cpu'")
    parser.add_argument("--patience", type=int, default=10,
                        help="Early stopping patience in epochs")
    parser.add_argument("--mode", type=str, choices=["cls", "reg", "joint"],
                        default="joint",
                        help="Training mode: 'cls' = classification only, "
                             "'reg' = regression only, 'joint' = multitask")
    parser.add_argument("--run_all", action="store_true",
                        help="If set, run all three configurations (cls/joint/reg)")

    args = parser.parse_args()

    if args.run_all:
        # use args as base config
        run_three_experiments(args)
    else:
        # single run
        run_experiment(
            data_dir=args.data_dir,
            csv_path=args.csv,
            epochs=args.epochs,
            lr=args.lr,
            lambda_reg=args.lambda_reg,
            device_str=args.device,
            patience=args.patience,
            mode=args.mode,
        )

if __name__ == "__main__":
    main()
    # Optional: perform data analysis and save plots/CSV.
    # data_analysis()
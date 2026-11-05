# Study guide: credit scoring project ("Ready to Spend")

My personal notes to understand and explain every part of the project.
Not part of the deliverable (the `notes/` folder is in `.gitignore`).

---

## 0. The mission in 30 seconds

"Ready to Spend" lends money to people with little or no credit history. They want a tool that:

1. **Predicts the probability** that a client will not repay (default), then **accepts or refuses** the loan.
2. **Explains** the score, in general and for each client, so that a research manager (and the client) can understand it.
3. Is built with an **MLOps** approach: every experiment tracked, the model versioned, and ready to go to production.

So the project has 3 blocks:

| Block | Question | Where |
|---|---|---|
| Model | Who will default? | notebooks 01, 02, 03 |
| Interpretability | Why this score? | notebook 04 |
| MLOps | How do we track, version, deploy and monitor it? | MLflow in 03/04, then API, tests, CI/CD, drift (still to do) |

**One sentence to remember:** "I built a LightGBM model that predicts the probability of default from 7 data sources, tuned it and chose its decision threshold to minimise a business cost where a missed default costs 10 times more than a refused good client, explained it with SHAP, and tracked and versioned everything with MLflow."

---

## 1. What is MLOps, and where does each piece fit?

**MLOps** = the practices that take a model from a notebook to production, and keep it working there. Same idea as DevOps for software, plus the problems specific to ML (data changes, experiments, model versions).

The life cycle of a model, and where my project is:

```
 1. Understand the data      -> notebook 01 (EDA)                      DONE
 2. Prepare the data         -> notebook 02 (cleaning, joins, features) DONE
 3. Experiment               -> notebook 03 + MLflow Tracking           DONE
 4. Tune & choose            -> notebook 03 (Optuna, threshold)         DONE
 5. Version the model        -> MLflow Model Registry (champion)        DONE
 6. Explain                  -> notebook 04 (SHAP)                      DONE
 7. Serve                    -> API (FastAPI) loading the champion      TO DO
 8. Test & automate          -> pytest + GitHub Actions (CI/CD)         TO DO
 9. Deploy                   -> cloud platform                          TO DO
10. Monitor                  -> data drift with Evidently               TO DO
```

**Why it matters:** without MLOps, a model is a notebook that only works on my computer, and nobody can say which version is in production, how it was trained or if it still works. With MLOps, anyone can reproduce the results, see the history of experiments, and replace the model safely.

**The 3 MLOps ideas I apply everywhere:**
- **Reproducibility**: fixed `random_state=42`, `requirements.txt`, the same split recreated in notebook 04, data processing in notebooks that can be re-run from the raw files.
- **Traceability**: every experiment is a run in MLflow with its parameters, metrics, tags and description.
- **Separation of steps**: one notebook per step, each one saving its output (Parquet files, registered model) for the next one.

---

## 2. Project map

```
Projet6-OC/
├── data/raw/                  the 8 CSV files from Kaggle (not in git, too big)
├── data/processed/            train_features.parquet, test_features.parquet (made by notebook 02)
├── notebooks/
│   ├── 01_eda.ipynb                     explore the data
│   ├── 02_feature_engineering.ipynb     clean, encode, join, create features
│   ├── 03_modeling.ipynb                models, MLflow, Optuna, threshold, registry
│   ├── 04_interpretability.ipynb        global and local importance (SHAP)
│   ├── images/home_credit.png           diagram of the tables
│   ├── start-here-a-gentle-introduction.ipynb   Kaggle kernel 1 (EDA)
│   └── lightgbm_with_simple_features.py          Kaggle kernel 2 (features)
├── mlflow.db                  MLflow: runs, params, metrics, registry (SQLite)
├── mlruns/                    MLflow: artifacts (plots, model files)
├── requirements.txt           exact versions of every library (pip freeze)
└── .gitignore                 what git must not track (data, venv, MLflow store, notes)
```

**Order to run:** 01 (about 1 min) → 02 (about 3 min) → 03 (about 31 min) → 04 (about 1 min).
03 needs the Parquet files from 02. 04 needs the `champion` model from 03.

**Environment:** `source myenv/bin/activate`. To recreate it elsewhere: `pip install -r requirements.txt`.

---

## 3. Notebook 01 - EDA (Exploratory Data Analysis)

**Goal:** understand the data before touching it.

### The data: 7 sources, 8 files

| File | One row = | Key |
|---|---|---|
| `application_train/test` | 1 loan application (the client **today**) | `SK_ID_CURR` |
| `bureau` | 1 loan at **another** institution | `SK_ID_CURR`, `SK_ID_BUREAU` |
| `bureau_balance` | 1 month of 1 bureau loan | `SK_ID_BUREAU` |
| `previous_application` | 1 previous application at **Home Credit** | `SK_ID_CURR`, `SK_ID_PREV` |
| `POS_CASH_balance` | 1 month of 1 previous shop/cash loan | `SK_ID_CURR`, `SK_ID_PREV` |
| `credit_card_balance` | 1 month of 1 previous credit card | `SK_ID_CURR`, `SK_ID_PREV` |
| `installments_payments` | 1 payment (made or missed) | `SK_ID_CURR`, `SK_ID_PREV` |

- **Keys** = columns used to connect tables. `SK_ID_CURR` is the client/application ID.
- The main table has **1 row per client**, all the others have **many rows per client** → they must be aggregated before joining (notebook 02).
- "Behavioral data" = the monthly/payment tables: what the client **did**, not what they declared.

### What I found, and what I decided

| Finding | Numbers | Decision |
|---|---|---|
| **Imbalanced target** | 8.1% defaults (TARGET = 1) | accuracy is useless (92% by always saying "repays") → ROC AUC + business cost; class weights / resampling |
| **No duplicates** | 0 duplicated rows, 0 duplicated IDs, 0 clients in both train and test | `SK_ID_CURR` is a safe key |
| **Many missing values** | 67 columns; 47% of clients have no building information at all | keep the columns (see next line) |
| **Missing = information** | e.g. no `OWN_CAR_AGE` (no car): 8.5% default vs 7.2% | don't drop columns; LightGBM keeps `NaN`; median imputation only for models that need it |
| **Mean vs median** | income: mean 168,798, median 147,150, max 117,000,000 | median imputation (robust to extreme values) |
| **Anomaly `DAYS_EMPLOYED = 365243`** | 55,374 clients, all pensioners or unemployed; default 5.4% vs 8.7% | it means "no job": flag column `DAYS_EMPLOYED_ANOM` + replace with `NaN` (train and test) |
| **Strongest features** | `EXT_SOURCE_3` (-0.18), `EXT_SOURCE_2` (-0.16), `EXT_SOURCE_1` (-0.16), `DAYS_BIRTH` (+0.08) | all correlations are "very weak" (< 0.2), normal for this problem; the model combines many of them |
| **Age** | default 12.3% at 20-25 years, 3.7% at 65-70 | useful feature (but don't discriminate: support young clients instead) |
| **16 categorical columns** | `ORGANIZATION_TYPE` has 58 categories | encoding in notebook 02 |
| **Gender** | men 10.1% default, women 7.0% | sensitive variable → fairness question (see notebook 04) |
| **Other tables** | only 28% of clients have a credit card history | after joining, missing values are normal |

### Words to know
- **`DAYS_` columns** are negative: counted backwards from the application day. `/ -365` gives years.
- **Pearson correlation**: -1 to 1, linear relation only, not causation. Scale: < 0.2 very weak, 0.2-0.4 weak, 0.4-0.6 moderate, 0.6-0.8 strong, > 0.8 very strong.
- **KDE plot**: a smoothed histogram; one curve per TARGET value to compare the shapes.
- **missingno**: `msno.bar` (share present per column), `msno.matrix` (white = missing, shows blocks).

### Link to MLOps
Understanding the data is what makes the rest **justifiable**: every later decision (keep NaN, median, class weights, business cost) points back to a finding here.

---

## 4. Notebook 02 - Data preparation and feature engineering

**Goal:** one clean table, 1 row per client, with all 7 sources. Output: `data/processed/*.parquet`.

### The core idea: aggregate, then join
1. **Aggregate**: `groupby("SK_ID_CURR").agg(...)` → statistics per client (count, mean, max, sum, var...).
2. **Join**: `merge(..., on="SK_ID_CURR", how="left")`. `left` = keep every client, even those absent from the other table (they get `NaN`).

`bureau_balance` needs **2 levels**: per loan (`SK_ID_BUREAU`) → joined to `bureau` → per client.

### Step by step

| Step | What | Why |
|---|---|---|
| Train + test together (`pd.concat`) | same transformations for both, same columns | no need to "align"; not leakage because no step learns from the data or uses the TARGET |
| Remove 4 `XNA` rows | `CODE_GENDER = XNA` | 4 rows out of 307,511, keeps the column binary |
| Label encoding | 4 columns with exactly 2 values and no NaN | 0/1 is fine for 2 values |
| One-hot encoding | all other text columns | no fake order (important for linear models); nominal vs ordinal explained |
| `DAYS_EMPLOYED` fix | flag + NaN | from notebook 01 |
| 5 ratio features | `DAYS_EMPLOYED_PERC`, `INCOME_CREDIT_PERC`, `INCOME_PER_PERSON`, `ANNUITY_INCOME_PERC`, `PAYMENT_RATE` | a banker compares the loan to the client's situation, not raw amounts |
| bureau: active vs closed | separate statistics | a running loan ≠ a loan closed 10 years ago |
| previous: approved vs refused | separate statistics | being refused before is a signal |
| previous: 365243 in 5 `DAYS_` columns | → NaN | same "unknown date" code |
| installments: `DPD`, `DBD`, `PAYMENT_PERC`, `PAYMENT_DIFF` | days late / early, share paid, amount missing | payment behaviour is very predictive |
| 2,905 missed payments | kept as NaN (the kernel turned them into "0 days late") | "on time" would be the opposite of the truth |
| Checks | `inspect_table` on every table (shape, types, duplicates, missing); rows stay 356,251 after every merge | brief: no duplicates, no row loss |
| Infinite values | 61 → NaN | division by 0 in ratios |
| Clean column names | special characters → `_` | LightGBM refuses some characters |
| Remove 27 constant columns | mostly empty `_nan` one-hot columns | no information at all |
| `float32` | 1.74 GB → 0.95 GB | memory |
| Save Parquet | train 307,507 × 770, test 48,744 × 769 | small, fast, keeps types; next notebooks don't redo this |

**Result:** 768 features. 15 of the 20 features most correlated with the target come from the other tables (credit card, bureau, previous applications) → joining was worth it.

### Data leakage (be ready to explain)
**Leakage** = the model sees information it should not have at training time (the test data, the target, the future). Row-by-row and client-by-client transformations are safe. Steps that **learn** from data (median for imputation, mean/std for scaling) must be learned on the **training folds only** → done inside scikit-learn **pipelines** in notebook 03.

### Link to MLOps
A deterministic, re-runnable preparation step whose output is saved: this is the start of a **data pipeline**. In production, the API will need the **same** transformations for a new client.

---

## 5. Notebook 03 - Modeling, MLflow, Optuna, threshold, registry

### 5.1 The split
- 80% train / 20% **validation**, `stratify=y` (8.1% defaults in both), `random_state=42`.
- `application_test` has no TARGET, so it can't be used to measure anything.
- The validation set is used **once**, at the end. Everything else (comparisons, tuning, threshold) uses cross-validation on the train part.

### 5.2 Cross-validation
- **3-fold `StratifiedKFold`**: cut into 3 parts with the same share of defaults; train on 2, predict the 3rd, 3 times.
- `cross_val_predict` → one prediction per client, always from a model that did not see that client.
- **Why:** a single split can be lucky or unlucky. I also log the **AUC of each fold and its standard deviation** (`auc_std`): small std = robust.

### 5.3 The business cost (the heart of the project)

|  | predicted: repays | predicted: defaults |
|---|---|---|
| **really repays** | TN | **FP**: good client refused → lost interest |
| **really defaults** | **FN**: bad client accepted → lost money | TP |

`business cost = (10 × FN + 1 × FP) / number of clients` (assumption: a FN costs 10 times a FP). Lower = better.

**Threshold:** the model gives a probability; above the threshold → refused. I test every threshold from 0.01 to 0.99 and keep the cheapest. Example: the dummy model costs 0.807 = 10 × 8.07% (it accepts everyone, so every default is a FN).

### 5.4 Metrics (and why not accuracy)
- **ROC AUC**: how well the model ranks clients (defaults get higher probabilities). 0.5 = random, 1 = perfect. Independent of the threshold → good to compare models.
- **Business cost**: the decision metric.
- **Recall**: share of defaults caught = TP / (TP + FN). The key classic metric because FN is the expensive error.
- **Precision**: share of refused clients who really would default = TP / (TP + FP).
- **F1**: balance of precision and recall; treats FN = FP, so only a complement.
- **Accuracy**: misleading with 8% defaults.

### 5.5 Models compared (all with cross-validation, all logged in MLflow)

| Model | Family | Imbalance handling | AUC (± std) | Business cost | Best threshold | Cost at 0.5 | Recall | F1 | Time |
|---|---|---|---|---|---|---|---|---|---|
| Dummy | baseline | - | 0.500 | 0.807 | - | 0.807 | 0 | 0 | 1 s |
| Logistic regression | linear | class_weight | 0.769 ± 0.001 | 0.511 | 0.53 | 0.513 | 0.66 | 0.29 | 151 s |
| Random forest | bagging | class_weight (balanced_subsample) | 0.746 ± 0.001 | 0.544 | 0.44 | 0.570 | 0.65 | 0.27 | 177 s |
| XGBoost | boosting | scale_pos_weight ≈ 11.4 | 0.780 ± 0.001 | 0.499 | 0.46 | 0.505 | 0.68 | 0.29 | 122 s |
| MLP (64, 32) | neural network | undersampling | 0.741 ± **0.010** | 0.551 | 0.56 | 0.551 | 0.62 | 0.27 | 79 s |
| LightGBM | boosting | none | 0.779 ± 0.002 | 0.500 | **0.08** | **0.778** | 0.69 | 0.29 | 78 s |
| **LightGBM** | boosting | **class_weight** | **0.780 ± 0.002** | **0.498** | 0.48 | 0.498 | 0.70 | 0.29 | **54 s** |
| LightGBM | boosting | undersampling | 0.775 ± 0.003 | 0.505 | 0.51 | 0.506 | 0.70 | 0.28 | 15 s |
| LightGBM | boosting | SMOTE | 0.775 ± 0.002 | 0.507 | 0.09 | 0.790 | 0.69 | 0.28 | 146 s |

(3-fold stratified CV on the train part. "Time" = the 3 folds. Random forest limited to 200 trees, depth 12, because it is slow.)

**Lessons:**
1. Boosting (LightGBM, XGBoost) is the best family on this tabular data; they are almost tied.
2. All models are robust (AUC std 0.001-0.003) except the MLP (0.010). Neural networks are rarely the best on tabular data with many missing values.
3. **The threshold matters**: LightGBM without balancing costs 0.500 at its best threshold (0.08) but 0.778 at 0.5. Same for SMOTE.
4. Once the threshold is optimised, the 4 imbalance methods give almost the same cost (0.498-0.507): **the threshold does most of the work**.
5. F1 hardly changes between models (0.27-0.29) while the business cost does → F1 is not the right metric to choose.
6. I chose LightGBM + `class_weight`: lowest cost, fastest of the best models (54 s vs 122 s for XGBoost), nothing extra in production (no imputer, no SMOTE).

**autolog demo:** `mlflow.lightgbm.autolog()` logged 20 parameters, the AUC on an evaluation part (0.785) and the feature importance plots, without a single `log_param`.

**Imbalance methods explained:**
- `class_weight="balanced"`: errors on the rare class count more in training.
- **Undersampling**: randomly remove majority-class rows until balanced (fast, loses data).
- **SMOTE**: create synthetic minority rows between real neighbours (needs no NaN, slow).
- With `imblearn.Pipeline`, resampling is applied **only when training**, never when predicting, and only on the training folds.

### 5.6 MLflow (tracking)

| Concept | Meaning | In my project |
|---|---|---|
| Tracking URI | where MLflow stores things | `sqlite:///mlflow.db` + `mlruns/` |
| Experiment | folder of runs | `credit_scoring` (with a description) |
| Run | one try | one per model, one per Optuna trial, the final model |
| Parameter | setting chosen before training | `log_param(s)` |
| Metric | result after training | `cv_auc`, `cv_auc_std`, `cv_business_cost`, `cv_recall`, `cv_f1`, `best_threshold`, `duration_seconds`... |
| Artifact | file attached to a run | cost curve, confusion matrix, ROC curve, SHAP plots, Optuna history |
| Tags / description | annotations | `stage` (comparison / autolog_demo / tuning / tuning_trial / final), `model_family`, `mlflow.note.content` = description |
| Nested runs | runs inside a parent | Optuna: parent `lgbm_tuning_optuna`, 1 child per trial |
| autolog | automatic logging | `mlflow.lightgbm.autolog()` demo run; not used everywhere because it doesn't know the business cost and would log one run per CV fold |

**Commands:**
```bash
source myenv/bin/activate
mlflow ui --backend-store-uri sqlite:///mlflow.db     # from the project root
# open http://127.0.0.1:5000
```
In the UI: sort runs by `cv_business_cost`, filter with `tags.stage = "comparison"`, select runs → "Compare".

**Good practices (brief step 2):** virtual env (`myenv` + `requirements.txt`), every run annotated, the model versioned, only useful files logged (one model, no datasets).

### 5.7 Hyperparameter tuning with Optuna
- **Hyperparameters**: settings chosen before training (`n_estimators`, `learning_rate`, `num_leaves`, `max_depth`, `min_child_samples`, `subsample`, `colsample_bytree`, `reg_lambda`).
- **GridSearchCV** tries every combination: 3 values × 8 settings = 6,561 combinations × 3 folds → impossible.
- **RandomizedSearchCV**: random combinations, doesn't learn from previous tries.
- **Optuna (TPE)**: each new trial is chosen near the best ones so far → fewer trials needed.
- **Objective = business cost** at the best threshold (cross-validated), **not** AUC or accuracy.
- 15 trials on half of the train part (speed), each trial = a nested MLflow run. `TPESampler(seed=42)` → reproducible.
- Output: `study.best_params`, the optimisation history plot.
- **Result:** best cost 0.4982 at trial 12 (first trial: 0.524). Best settings: 350 trees, learning rate 0.032, 23 leaves, max depth 8, min_child_samples 194, subsample 0.87, colsample_bytree 0.30, reg_lambda 0.039. The best trials all have a small learning rate and many clients per leaf (min_child_samples ≈ 170-194) → the model prefers to be **cautious** (less overfitting).
- Total: about 13 minutes for 15 trials.

### 5.8 The final model and the threshold
1. Cross-validated predictions on the whole train part with the best parameters → **cost vs threshold curve** → best threshold. The threshold is chosen **without** the validation set (otherwise the final score would be too optimistic).
2. **Robustness:** best threshold, AUC and cost for each fold separately → they must be close.
3. Compare: tuned vs default settings (same CV), and cost at the best threshold vs at 0.5.
4. Train on the whole train part, evaluate **once** on the validation set: AUC, business cost, recall, precision, F1, cost at 0.5.
5. Confusion matrix and ROC curve.

**My results:**

| | Value |
|---|---|
| CV AUC (tuned) | 0.784 |
| CV business cost: default settings → tuned | 0.498 → 0.492 (small gain: most of the gain comes from class weights + threshold) |
| Best threshold | **0.49** |
| Per fold: AUC / threshold / cost | 0.784 / 0.48 / 0.494 · 0.781 / 0.51 / 0.493 · 0.786 / 0.52 / 0.485 → stable |
| Validation AUC | **0.786** (close to CV → no overfitting) |
| Validation business cost | **0.489** (at 0.5: 0.4875) |
| Validation recall / precision / F1 / accuracy | 0.713 / 0.183 / 0.291 / 0.720 |
| Training time | 54 s |

So: the model catches **71% of the defaults**; about 1 refused client in 5 would really have defaulted (precision 0.18) - that is the price of avoiding FN, which cost 10 times more.

**The 0.49 vs 0.5 question (be ready!):** on the validation set, 0.5 is even very slightly cheaper (0.4875 vs 0.4887). The difference (0.001) is smaller than the differences between folds (0.485-0.494): it is noise, the curve is flat between about 0.45 and 0.55. Honest answer: "With class_weight='balanced', 0.5 happens to be in the optimal zone; my analysis **proves** it with the curve and the folds instead of assuming it. Without class weights, keeping 0.5 would have cost 0.778 instead of 0.500."

**Why the threshold can be close to 0.5 here:** `class_weight="balanced"` already pushes the probabilities of risky clients up. Without class weights the best threshold was 0.08. → The threshold and the class weights are chosen **together**, and the **curve** justifies the choice, not the habit of 0.5.

### 5.9 Model Registry (versioning)
- `mlflow.lightgbm.log_model(..., registered_model_name="credit_scoring_lgbm")` → **version** 1, 2, 3... each time notebook 03 runs.
- **Signature**: list of input columns and types, saved with the model. All features are `float32` so that missing values are always accepted.
- **Alias `champion`** → the version used in production. To change the production model, move the alias; the code that loads `models:/credit_scoring_lgbm@champion` doesn't change.
- **Tag `threshold`** on the version: the threshold is **part of the model** (without it, a probability can't become a decision).
- **Description** on the version with its validation scores.
- Section 8 of the notebook reloads the champion and scores 5 test clients: this is exactly what the API will do.
- **In my project now:** `credit_scoring_lgbm` version 1, alias `champion`, tag `threshold = 0.49`, description "LightGBM, class_weight='balanced', threshold 0.49. Validation AUC 0.786, business cost 0.489." 27 runs in the experiment (9 comparison, 1 autolog demo, 1 tuning + 15 trials, 1 final). Store size: about 8 MB.

(Note: I use `mlflow.lightgbm`, not `mlflow.sklearn`, because the MLflow 3 scikit-learn flavour refuses LightGBM objects by default.)

---

## 6. Notebook 04 - Interpretability

**Why:** a refused client has the right to understand; the company must check the model is fair; the research manager needs to trust it. The mission asks for **global** and **local** importance.

| | Global | Local |
|---|---|---|
| Question | which features matter in general? | why this score for this client? |
| LightGBM gain | yes (total improvement brought by each feature) | no |
| SHAP | yes (mean of absolute SHAP values, beeswarm) | yes (waterfall, table) |

### SHAP in 4 points
1. From game theory: the features "share" the prediction.
2. `prediction = average prediction + sum of the SHAP values of the client`.
3. Positive SHAP value → pushes towards default; negative → towards repayment.
4. For LightGBM the unit is **log-odds**; `probability = 1 / (1 + exp(-log_odds))`. The notebook checks that SHAP gives back exactly the model's probability.

### How to read the plots
- **Bar**: mean |SHAP| per feature = global importance.
- **Beeswarm**: 1 dot per client; position = SHAP value (right = more risk), color = feature value (red high, blue low). E.g. red `EXT_SOURCE_3` dots on the left → high score lowers the risk.
- **Waterfall**: from the average prediction `E[f(X)]` (bottom) to the client's prediction `f(x)` (top); red bars push the risk up, blue down.
- **`explain_client` table**: feature, client's value, SHAP value, "increases / decreases risk" → what the API will return.

### What it showed (real champion, version 1)
SHAP top 10 (mean |SHAP|): `EXT_SOURCE_2`, `EXT_SOURCE_3`, `EXT_SOURCE_1`, **`CODE_GENDER`**, `DAYS_EMPLOYED`, `PAYMENT_RATE`, `APPROVED_CNT_PAYMENT_MEAN` (length of previous approved loans), `AMT_ANNUITY`, `INSTAL_DPD_MEAN` (late payments), `OWN_CAR_AGE`. Consistent with the EDA, and 2 of the top 10 come from the other tables (joining was useful).

Examples: client 209192 refused (probability 0.674 > 0.49), mainly because of a low `EXT_SOURCE_2`, being a man and a high `PAYMENT_RATE` (short loan). Client 267442 accepted (0.077), mainly because of high `EXT_SOURCE_2` and `EXT_SOURCE_3`, a low `PAYMENT_RATE` and a long employment.

**Fairness:** `CODE_GENDER` is **4th** (being a man increases the risk). Possible action: retrain without it and compare the business cost; if it barely changes, remove it.

Everything (plots + importance CSV) is attached to the **champion's run** in MLflow (`start_run(run_id=...)`), so the model and its explanations stay together.

---

## 7. The Kaggle kernels: what I took, what I changed

The mission allows Kaggle kernels **if I analyse and adapt them**. I used two:

**Will Koehrsen, "Start Here: A Gentle Introduction"** (EDA)
- Kept: target, missing values, types, `DAYS_EMPLOYED` anomaly, correlations, age, `EXT_SOURCE`, ROC AUC explanation, label vs one-hot.
- Dropped: polynomial features (didn't help: 0.678 with or without), random forest as main model, pairs plot, submission files.
- Kernel scores (main table only): logistic 0.671, random forest 0.678, LightGBM 0.735, LightGBM + domain features 0.754. Mine are higher (≈ 0.78) because I use all the tables.

**jsaguiar, "LightGBM with Simple Features"** (feature engineering)
- Kept: all the aggregations, active/closed, approved/refused, the 5 ratios, `DPD`/`DBD`.
- Changed: steps written one by one; 4 binary columns label-encoded (not 3 chosen by hand); missed payments kept as NaN; `clip` instead of `apply(lambda)`; added the anomaly flag, inf → NaN, clean names, constant columns, checks.
- Moved to notebook 03: the model (the kernel uses 10 non-stratified folds, early stopping, parameters from another kernel, no business cost).

**Kernel code that breaks with recent libraries (tested on my setup):**
- pandas 3 stores text as `str`, so `dtype == 'object'` is always False → the kernels' label encoding and `one_hot_encoder` silently encode **nothing**.
- `df['col'].replace(..., inplace=True)` does nothing in pandas 3 (copy-on-write); pandas warns, but both kernels hide warnings.
- `df.append` removed → `pd.concat`. `df.corr()` crashes with text columns.
- `sklearn.preprocessing.Imputer` removed → `sklearn.impute.SimpleImputer`; `get_feature_names` → `get_feature_names_out`.
- LightGBM 4: `fit(..., early_stopping_rounds=...)` crashes → callbacks.

**What to say:** "I didn't copy the kernels: I checked every step against my library versions, found code that silently does nothing in pandas 3, fixed the handling of missed payments, and added the business cost, which a Kaggle competition doesn't need."

---

## 8. Every key decision in one table

| Decision | Why | Alternative I rejected |
|---|---|---|
| ROC AUC + business cost, not accuracy | 8% defaults; FN costs more than FP | accuracy (92% by doing nothing) |
| FN = 10 × FP | lending money to a defaulter loses the capital, refusing a good client loses the interest | equal costs (F1) |
| Keep columns with missing values | "missing" predicts default | dropping them |
| LightGBM keeps NaN; median elsewhere | LightGBM handles NaN; median is robust | mean (pulled by extreme values) |
| Train + test encoded together | identical columns, no alignment | separate encoding + `align` |
| One-hot for all multi-category columns | no fake order, works for linear and tree models | label encoding (fake order), ordinal encoding (assumes equal distances) |
| Aggregate then left-join | 1 row per client, no client lost | inner join (loses clients) |
| Imputation/scaling inside pipelines | learned on training folds only → no leakage | fitting them on all the data |
| Stratified 3-fold CV + per-fold std | robust and comparable estimates | single split |
| LightGBM + `class_weight` | best cost, fast, simple in production | SMOTE (slower, needs imputation), undersampling (loses data) |
| Optuna on the business cost | learns from previous trials; optimises what the company cares about | GridSearch (too many combinations), tuning on AUC |
| Threshold from the cost curve on CV predictions | 0.5 is arbitrary; validation set kept clean | 0.5 by default; threshold tuned on validation (optimistic) |
| Threshold stored with the model (tag) | the API needs it | threshold hard-coded in the API |
| `mlflow.lightgbm` flavour | the sklearn flavour refuses LightGBM in MLflow 3 | |
| SHAP | global + local + direction of the effect | built-in importance only (global, no direction) |

---

## 9. Numbers to remember

- 307,511 applications in train (122 columns), 48,744 in test; **8.1% defaults**.
- 7 data sources, 8 files; `installments_payments` 13.6 M rows, `bureau_balance` 27.3 M rows.
- 55,374 clients with `DAYS_EMPLOYED = 365243`.
- Final dataset: **768 features**, 307,507 train clients.
- Dummy cost 0.807; LightGBM + class_weight: AUC 0.780, cost 0.498 (CV).
- Optuna: 15 trials, best CV cost 0.498 on the search data; tuned model CV cost 0.492.
- **Final: threshold 0.49, validation AUC 0.786, business cost 0.489, recall 0.71** (cost at 0.5: 0.4875).
- Notebook 03 takes about 31 minutes.

---

## 10. What is left (the rest of the MLOps mission)

| Step | What | How it connects to what exists |
|---|---|---|
| **API** | FastAPI app: input = a client ID (or features), output = probability, decision, top SHAP reasons | loads `models:/credit_scoring_lgbm@champion` and the `threshold` tag; reuses `explain_client` logic from notebook 04 |
| **Feature pipeline in code** | move notebook 02's logic into a Python module | the API must transform a new client exactly like the training data |
| **Tests** | pytest: the API answers, probability in [0, 1], decision consistent with the threshold, features have the right columns | protects against regressions |
| **CI/CD** | GitHub Actions: run tests on every push, then deploy | automation = no manual mistakes |
| **Deployment** | cloud platform | the "pre-production" of the mission |
| **Data drift** | Evidently report comparing training data vs new data (`application_test` as "production") | detects when the model needs retraining |
| (optional) Dashboard | Streamlit page calling the API | for the research manager |

---

## 11. Questions I may be asked (and my answers)

**Why not accuracy?** With 8% defaults, a model that accepts everyone has 92% accuracy and catches no defaults.

**What is your business cost?** (10 × FN + FP) / clients. A missed default loses the money lent; a refused good client only loses the interest. 10 is an assumption that the business should confirm; the code makes it a single parameter (`FN_COST`).

**How did you choose the threshold?** I computed the business cost for every threshold from 0.01 to 0.99 on cross-validated predictions of the train part, took the minimum, checked it is stable on each fold, and only then evaluated once on the validation set.

**Why is your threshold close to 0.5 then?** Because `class_weight="balanced"` already shifts the probabilities. Without class weights it was 0.08. The curve justifies it.

**How did you handle the imbalance?** Compared class weights, undersampling and SMOTE on LightGBM; after threshold optimisation they were almost equal, so I kept class weights (simplest, fastest, nothing extra in production).

**How do you avoid data leakage?** Imputation, scaling and resampling are inside pipelines fitted on training folds only; the validation set is used once; the threshold is chosen without it; train and test are only transformed row by row or client by client in notebook 02.

**Why cross-validation?** A single split depends on luck. 3 stratified folds give 3 estimates; I report the mean and the standard deviation.

**Why Optuna?** GridSearch would need 6,561 combinations × 3 folds. Optuna learns from previous trials and I gave it the business cost as the objective.

**What does MLflow give you?** A record of every experiment (parameters, metrics, plots, tags, descriptions), a UI to compare runs, and a registry with versions and an alias so the API always loads the right model.

**What is the difference between a run, an experiment and a registered model?** Experiment = folder; run = one training with its logs; registered model = a named, versioned model selected from a run for production.

**What is the `champion` alias?** A pointer to the production version. Changing the production model = moving the alias, no code change.

**Global vs local importance?** Global: which features matter overall (SHAP bar/beeswarm, LightGBM gain). Local: why this client got this score (SHAP waterfall, `explain_client`).

**What is a SHAP value?** The contribution of a feature to one prediction, compared to the average prediction. They add up exactly to the model's output (log-odds).

**Is the model fair?** `CODE_GENDER` is important. I'd test a model without it; if the cost barely changes, remove it. Age is also important, but it's a strong legitimate risk factor; the company should support young clients rather than simply refuse them.

**What would you improve?** Feature selection (768 features, many redundant, e.g. `_AVG`/`_MODE`/`_MEDI`), 5-fold CV, more Optuna trials, early stopping, calibration of the probabilities, confirming the 10:1 cost with the business.

**What did you take from Kaggle and what did you change?** See section 7.

---

## 12. Glossary

- **Aggregation**: summarising many rows into one (mean, max, count...).
- **Alias (MLflow)**: a name (e.g. `champion`) pointing to a model version.
- **Artifact**: a file attached to an MLflow run.
- **AUC (ROC)**: area under the ROC curve; ranking quality, 0.5 random, 1 perfect.
- **Bagging**: many independent models averaged (random forest).
- **Boosting**: models trained one after the other, each correcting the previous (LightGBM, XGBoost).
- **Class weight**: makes errors on the rare class count more.
- **Confusion matrix**: TN, FP, FN, TP.
- **Cross-validation**: repeated train/test on different parts of the data.
- **Data drift**: the new data no longer looks like the training data.
- **Data leakage**: the model learns from information it shouldn't have.
- **Early stopping**: stop training when the validation score stops improving.
- **Feature engineering**: creating new useful columns.
- **Hyperparameter**: a setting chosen before training.
- **Imputation**: filling missing values.
- **Label / one-hot / ordinal encoding**: text → numbers (one number per category / one 0-1 column per category / numbers in a meaningful order).
- **Log-odds**: log(p / (1 - p)); the internal scale of LightGBM.
- **MLOps**: practices to bring ML models to production and keep them working.
- **Nested run**: a child run inside a parent run.
- **Overfitting**: the model learns the training data by heart and generalises badly.
- **Parquet**: compressed column-based file format, keeps types.
- **Pipeline**: chain of steps (impute → scale → model) fitted together.
- **Precision / recall / F1**: TP/(TP+FP) / TP/(TP+FN) / their harmonic mean.
- **Registry**: catalogue of versioned models.
- **Run**: one tracked experiment in MLflow.
- **SHAP**: method to split a prediction into feature contributions.
- **Signature**: input/output schema saved with an MLflow model.
- **SMOTE**: creates synthetic minority-class rows.
- **Stratified**: keeps the same class proportions in each part.
- **Threshold**: probability above which a loan is refused.
- **TPE**: Optuna's algorithm, chooses new trials near the best ones.
- **Undersampling**: removes majority-class rows to balance.

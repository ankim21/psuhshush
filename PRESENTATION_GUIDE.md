# Presentation guide: the 4 notebooks (for a complete beginner)

A walkthrough for presenting the credit scoring project to someone who has **never done machine learning**.
Every technical word is explained the first time it appears. A glossary is at the end.

---

## 0. Start here: the story in plain words

**The situation.** A company called "Ready to Spend" lends money to people who have little or no credit history. Some people pay the money back. Some don't. The company wants a tool that looks at a new application and says: *"this person has a 67% chance of not paying back, so refuse"* or *"7%, so accept"*.

**How do we build that tool?** We don't write the rules by hand ("refuse if income < X"). Instead, we show a computer **307,000 past loans where we already know the answer** (repaid or not), and it learns the patterns by itself. That is **machine learning**. The result is called a **model**: a "recipe" that takes a client's information and outputs a probability.

**The 4 notebooks are 4 steps, like cooking:**

| Notebook | Cooking analogy | What it does | What it produces |
|---|---|---|---|
| 01 – EDA | Look at the ingredients | Explore the data, find problems | Nothing saved, just understanding |
| 02 – Feature engineering | Wash, cut, mix | Clean the data, gather 8 files into 1 table | `data/processed/*.parquet` |
| 03 – Modeling | Try recipes, pick the best | Train 9 models, compare them, choose the best, tune it | A saved model called `champion` |
| 04 – Interpretability | Explain the taste | Explain *why* the model accepts or refuses someone | Explanation charts |

Each step saves its result so the next one can start from it. Tell the student early: this is how real projects stay organised.

---

## 1. Notebook 01: Exploratory Data Analysis (EDA)

**One sentence:** *"Before building anything, find out what is in the data, what is broken, and what seems useful."*

### Words to explain first
- **Table / DataFrame:** like an Excel sheet. One **row** = one client, one **column** = one piece of information (income, age…).
- **TARGET:** the column we want to predict. `1` = the client had trouble repaying, `0` = repaid normally.
- **Feature:** any other column the model can use to make its guess.

### Walkthrough

| Section | What to show | What to say |
|---|---|---|
| 1. The data | The diagram of tables | "We have 8 files. The main one has 1 row per loan application. The others hold the client's history: old loans, monthly payments, credit cards… They are linked with ID numbers, like a client number." |
| 3. File sizes | The row counts | "`bureau_balance` has 27 million rows. Each client appears many times in these files, so we will have to summarise them (notebook 02)." |
| 4. Main table | `app_train.head()` | 307,511 clients, 122 columns. The test file has the same columns minus TARGET: these are the "new" clients we would have to predict. |
| 4. Duplicates | The checks | "No client appears twice, and none appears in both files. Good: we can trust the ID." |
| 5. The target | Bar chart | **Only 8% of clients default.** This is the most important fact of the whole project (see below). |
| 6. Missing values | The `missingno` charts | 67 columns have empty cells. Empty cells come in **blocks**: when a client has no info about their building, all 47 building columns are empty at once (47% of clients). |
| 6. Is empty informative? | The default rates | Clients with an empty value default **more**. "Empty" is information. Example: no car age means no car. So we don't delete these columns. |
| 6. Mean vs median | Income: mean 169k, median 147k, max **117 million** | "One client claims 117 million. The *average* gets pulled up by this one person. The *median* (the middle value) does not. So when we fill empty cells, we use the median." |
| 7. Column types | 16 text columns | Models only understand numbers, so text like "Married" must be turned into numbers (**encoding**, done in notebook 02). |
| 8. Anomalies | `DAYS_EMPLOYED` histogram | Some clients have "employed for 1,000 years". All 55,000 of them are pensioners or unemployed: it is a **code for "no job"**, not a typo. We keep that info in a new yes/no column and blank out the fake number. |
| 9. Correlations | The top/bottom list | A **correlation** says "when this column goes up, does the default rate go up or down?" (from -1 to +1). All are weak (< 0.2): no single column predicts default by itself. The best ones are the 3 external credit scores (`EXT_SOURCE_1/2/3`) and age. |
| 10. Age | Default rate by age group | **12% of 20–25 year olds default vs 3.7% of 65–70 year olds.** The easiest chart for a beginner to read. |
| 11. External scores | Heatmap + curves | Higher score means less default. These will be the model's most important features (confirmed in notebook 04). |
| 12. Categories | Default rate per group | Men 10.1% vs women 7.0%. Groups with only 5 people (e.g. "Maternity leave") can't be trusted: one person changes everything. |
| 13. Coverage | % of clients in each file | Only 28% of clients ever had a credit card here, so the others will have empty credit-card columns. |

### Focus: what is `EXT_SOURCE`?

These 3 columns are the **most important features of the whole project** (top 3 in notebook 04), so the student will ask about them.

**What we know:** the official description file only says *"Normalized score from external data source"*. Home Credit never revealed which source. They are almost certainly **credit scores bought from outside organisations**, such as credit bureaus, telecom operators or other lenders. Think of them like a FICO score in the US or a Banque de France rating, but rescaled.

- **"Normalized"** = rescaled to be between 0 and 1. **Higher = more trustworthy client.**
- They are **black boxes**: we use them, but we don't know how they were calculated.

**What the data shows (train set):**

| | `EXT_SOURCE_1` | `EXT_SOURCE_2` | `EXT_SOURCE_3` |
|---|---|---|---|
| Empty cells | **56%** | 0.2% | 20% |
| Median value | 0.51 | 0.57 | 0.54 |
| Default rate, lowest 20% of scores | 14.6% | 15.2% | 16.4% |
| Default rate, highest 20% of scores | 3.0% | 3.6% | 3.4% |
| Link with age | **strong (0.60)** | almost none (0.09) | weak (0.21) |

How to read it: clients in the worst 20% of `EXT_SOURCE_3` default **5 times more often** than clients in the best 20%. No other column comes close.

**Three things to point out:**
1. **They are 3 different sources.** They're only weakly related to each other (correlations 0.11 to 0.21), so each one brings new information. That's why the model uses all three.
2. **`EXT_SOURCE_1` partly contains age:** older clients get higher scores. Whoever built it probably used age as an input.
3. **Only 0.06% of clients have all 3 empty.** Almost everyone has at least one score, which is why they are so useful.

**Discussion point for the student:** our model's best features are *someone else's* scores. That's practical, but:
- if a provider changes its formula or stops selling the data, our model breaks;
- we can't fully explain a refusal ("your external score was low… computed by someone, somehow");
- if those scores contain unfairness, our model inherits it.

### The 3 ideas a beginner must leave with
1. **Imbalance: why "accuracy" lies.** If the model says "everyone repays", it is right 92% of the time and still catches **zero** bad clients. So we never judge the model by accuracy. We use:
   - **ROC AUC**: "If I pick one bad client and one good client at random, how often does the model give the bad one a higher risk score?" 0.5 = coin flip, 1.0 = perfect.
   - **Business cost**: how much money the mistakes cost (explained in notebook 03).
2. **Missing doesn't mean useless.** Investigate before deleting.
3. **Weird values are often codes.** Investigate before deleting.

### Questions to ask the student
1. A model is right 92% of the time. Is it good? *(Not necessarily: "always say repays" also gets 92%.)*
2. `DAYS_BIRTH` is counted backwards (e.g. -10,000 days). It has a *positive* correlation with default. Do older people default more? *(No: a bigger number like -8,000 means younger, so younger people default more.)*
3. Why not delete every column that is more than 60% empty?

---

## 2. Notebook 02: Data preparation and feature engineering

**One sentence:** *"Turn 8 messy files, where clients appear many times, into 1 clean table with exactly 1 row per client."*

### The key idea: summarise, then attach
Use the toy example at the top of the notebook (3 clients). Draw it on a whiteboard:

```
Client 1 had 3 old loans: 1000, 5000, 3000   ->  mean 3000, max 5000, count 3
Client 2 had 1 old loan: 20000               ->  mean 20000, max 20000, count 1
Client 3 had no old loans                    ->  empty, empty, empty
```

1. **Summarise (aggregate):** squash many rows per client into 1 row of statistics (mean, max, count…).
2. **Attach (join / merge):** glue these new columns onto the main table using the client ID.

`how="left"` means "keep every client, even those with no history" (client 3 gets empty cells). Ask: *"What if we dropped client 3 instead?"* → we would lose clients, and new clients without history could never be scored.

### Walkthrough

| Section | What happens, in plain words | Table size after |
|---|---|---|
| 3. Main table | Stack train + test so both get exactly the same treatment. Remove 4 clients with gender "XNA". Turn text into numbers (see below). Fix the 365243 code. | 356,251 rows × 241 columns |
| 3. New features | Create 5 **ratios** a banker would look at (see below) | |
| 4. Other banks' loans (`bureau`) | Two-step summary: monthly history → per loan → per client. Separate summaries for **ongoing** and **finished** loans | 363 columns |
| 5. Past applications here (`previous_application`) | Same 365243 code in 5 date columns → blank. Separate summaries for **accepted** and **refused** past applications | 612 |
| 6. Shop/cash loans (`POS_CASH`) | How many days late, on average and at worst | 630 |
| 7. Payment history (`installments`) | For every payment: what share was paid, how many days late or early | 656 |
| 8. Credit cards | Min, max, mean, sum and spread of every column | 797 |
| 9. Cleaning | Check nothing got duplicated; replace "infinity" (division by 0) with empty; fix column names; remove 27 columns that are always the same value; halve the memory (1.74 → 0.95 GB) | 770 |
| 10. Quick check | Among the 20 columns most linked to default, **15 come from the history files**. Gathering the files was worth it. | |
| 11. Save | Split train/test back apart and save in **Parquet** (a faster, smaller format than CSV) | train: 307,507 × 770 |

### Turning text into numbers (encoding)
- **2 categories** (e.g. car: Yes/No) → simply 0 / 1. This is **label encoding**.
- **More than 2** (e.g. family status) → one yes/no column per category. This is **one-hot encoding**:

```
FAMILY_STATUS      ->   Married  Single  Widow
Married                    1       0       0
Single                     0       1       0
```

Why not "Married = 1, Single = 2, Widow = 3"? Because the model would think Widow is "bigger" than Married, which is meaningless.

### The 5 banker ratios
| Feature | In plain words |
|---|---|
| `INCOME_CREDIT_PERC` | income ÷ loan amount: "how big is the loan compared to what you earn?" |
| `ANNUITY_INCOME_PERC` | yearly repayment ÷ income: "what share of your salary goes to this loan?" |
| `PAYMENT_RATE` | yearly repayment ÷ loan amount: high = short loan, low = long loan |
| `DAYS_EMPLOYED_PERC` | time in current job ÷ age: "how stable is your job?" |
| `INCOME_PER_PERSON` | income ÷ family size |

The example to use: *"A 500k loan is no problem if you earn 300k a year, but it is if you earn 30k."* A ratio captures that, the raw numbers don't. (Notebook 04 will show that `PAYMENT_RATE` became the 4th–6th most important feature.)

### Concepts worth a minute each
- **Data leakage** = letting the model "peek" at information it won't have in real life, which makes it look better than it is. Explained in detail just below.

### Focus: train vs test, and where leakage can sneak in

**How the data is separated (3 times):**

```
application_train.csv (has TARGET)        application_test.csv (NO TARGET)
            └──── notebook 02: stacked, same features built ────┘
                                  │
                split back apart: "is TARGET empty?"
            ┌─────────────────────┴─────────────────────┐
   train_features.parquet                    test_features.parquet
            │                                (only used at the end of 03
   notebook 03: 80/20 split                   to score 5 "new" clients)
     ┌──────┴──────┐
  X_train      X_valid  ← final exam, locked until the end
     │
  3-fold CV inside X_train (mock exams)
```

**Do new features go only into training? No.** Every feature must exist for **every** client. The model expects the same 768 columns for anyone it scores. What must stay train-only is *learning numbers from a group of clients*.

| | Type A: uses only this client's own data | Type B: learns something from many clients |
|---|---|---|
| Examples | `PAYMENT_RATE`, the 365243 flag, mean of *this client's* past loans, days late on *this client's* payments | Median for empty cells, `StandardScaler`, SMOTE, the threshold, Optuna's settings |
| Rule | Apply to everyone, any time (notebook 02) | Learn on training data only, then reuse on the others (notebook 03, inside the `Pipeline`) |
| Why | A client's `PAYMENT_RATE` is the same whether or not other clients are in the table | If validation clients helped compute the median, they helped build the model they're graded on, like seeing the exam answers |

**The test to give the student:** *"Could I compute this for a brand-new client, alone, on the day they apply?"*
- Yes → Type A, apply it to everyone.
- No, it needs other clients → Type B, learn it on training data only.
- And if it uses **other clients' TARGET** (e.g. "default rate of your profession"), it's the most dangerous kind and must *only* be learned on training data. Notebook 02 never does this.

**Two harmless grey areas in notebook 02, worth mentioning to be honest:**
- One-hot encoding takes its list of categories from train + test. A category seen only in test gives an always-0 column in train, and the model just ignores it.
- Constant columns are detected on train + test together. At worst, a useless column is kept.

Neither uses TARGET, and neither can make the scores look better than they are.
- **The average of a 0/1 column is a percentage.** `STATUS_1_MEAN = 0.2` means "1 month late on 20% of months".
- **Don't copy code blindly:** the Kaggle code this is based on turned *missed* payments into "0 days late", which means "paid on time". The notebook fixes that.

### Questions to ask the student
1. Why summarise the monthly history by *loan* first, and only then by *client*?
2. Why is "number of refused past applications" probably useful?
3. After all these merges, how do you prove that no client was duplicated? *(Count rows and unique IDs: both 356,251.)*

---

## 3. Notebook 03: Modeling (the heart of the project)

**One sentence:** *"Try several types of model, judge them by how much money their mistakes cost, pick the best, fine-tune it, and save it."*

### Step 1: Hold some data back (the "exam")
- 80% of clients = **training set**: the model learns from these.
- 20% = **validation set**: locked in a drawer until the very end. It is the final exam, made of questions the model has never seen.
- **Stratified** = both parts keep exactly 8% defaulters.

**Cross-validation (CV)**, to compare models fairly before the exam: cut the training set into 3 parts. Train on 2, test on the 3rd, and rotate 3 times. Every client gets a prediction from a model that never saw them. It's like 3 mock exams instead of 1: less dependent on luck.

### Step 2: The business cost (what mistakes cost)
The model outputs a probability. To decide, we pick a **threshold**: above it → refuse.

| | Model says "repays" | Model says "defaults" |
|---|---|---|
| **Really repays** | ✅ correct | ❌ **False Positive**: we refused a good client → lost some interest |
| **Really defaults** | ❌ **False Negative**: we lent to a bad client → **lost the money** | ✅ correct |

A false negative is much worse. Assumption: **it costs 10 times more.**

`business cost = (10 × false negatives + 1 × false positives) / number of clients` → **lower is better.**

The notebook tries every threshold from 0.01 to 0.99 and keeps the cheapest one. Ask: *"Why not just use 50%?"* The answer comes from the results.

### Step 3: The models, explained for a beginner

All models answer the same question ("what is the probability this person defaults?") but they "think" differently. Present them from simplest to most powerful.

#### 🎲 Dummy model (the "reference point")
- **What it does:** ignores the client completely and always answers "8%" (the average default rate).
- **Analogy:** a weather forecaster who says "it rains 8% of days" every single day.
- **Why use it:** it is the *minimum bar*. Any real model must beat it, otherwise it learned nothing.
- **Result:** AUC 0.50 (coin flip), cost 0.807. It accepts everyone and misses every bad client.

#### 📏 Logistic regression (the "points scorecard")
- **What it does:** gives each feature a weight (plus or minus points), adds them all up, and turns the total into a probability between 0 and 1.
  - e.g. high external score → **−3 points**, young → **+1 point**, short job → **+1 point**… total → probability.
- **Analogy:** an old-fashioned bank scorecard where each answer earns or loses points.
- **Strengths:** simple, fast, easy to explain.
- **Weaknesses:** it can only learn "straight-line" effects ("more X = more risk, always at the same rate"). It cannot handle empty cells, so they are filled with the median first, and all columns are rescaled to the same range (**StandardScaler**) so that a column in millions doesn't overpower a column between 0 and 1.
- **Result:** AUC 0.769, cost 0.511. **Respectable** for something this simple.

#### 🌳 Decision tree (not tested alone, but needed to understand the next 3 models)
- **What it does:** asks a chain of yes/no questions, like the game "20 Questions":

```
External score < 0.4 ?
 ├─ yes → Age < 30 ?
 │         ├─ yes → risk 25%
 │         └─ no  → risk 12%
 └─ no  → Late payments before ?
           ├─ yes → risk 10%
           └─ no  → risk 3%
```

- **Strengths:** very intuitive, handles empty cells, captures combinations ("young **and** low score").
- **Weakness:** a single tree is unstable: it memorises the training data (**overfitting**, like a student who memorises past exam answers instead of understanding the subject).
- The fix is to use **many trees**. There are two ways to do that ↓

#### 🌲🌲🌲 Random forest (many trees **voting**)
- **What it does:** builds 200 trees **independently**. Each tree sees a random sample of clients and a random subset of columns. The final answer is the **average** of all trees. This is called **bagging**.
- **Analogy:** asking 200 different people for their opinion and averaging. Individual mistakes cancel out.
- **Result:** AUC 0.746, cost 0.544. Not great **here**, because the trees had to be limited (max depth 12) to keep training time reasonable on 768 columns.

#### 🚀 Gradient boosting: XGBoost and LightGBM (many trees **learning from each other's mistakes**)
- **What it does:** builds trees **one after the other**. Tree 1 makes a guess. Tree 2 focuses on **what tree 1 got wrong**. Tree 3 corrects what's left, and so on for hundreds of trees. The final answer is the sum of all of them.
- **Analogy:** a student doing practice exams. After each one, they study **only the questions they got wrong**.
- **Random forest vs boosting:** forest = 200 people working **in parallel** and voting. Boosting = a team working **in sequence**, each fixing the previous person's errors.
- **Strengths:** usually the best on table-shaped data like ours. Handles empty cells by itself, no rescaling needed.
- **XGBoost vs LightGBM:** two popular implementations of the same idea. LightGBM is built to be faster.
- **Results:** XGBoost AUC 0.780 / cost 0.499; LightGBM AUC 0.780 / cost **0.498**. **The two best models, almost tied.**

#### 🧠 Neural network (MLP, "multi-layer perceptron")
- **What it does:** layers of tiny calculators ("neurons"). Each one takes a weighted sum of its inputs and passes the result to the next layer. Here: input (768 columns) → 64 neurons → 32 neurons → probability.
- **Analogy:** it's like stacking many small logistic regressions on top of each other, so it can learn curved, complex patterns.
- **Why it's not the winner here:** neural networks shine on images, sound and text. On tables with lots of empty cells, tree-based boosting usually wins. It also needs empty cells filled and columns rescaled.
- **Result:** AUC 0.741, cost 0.551, and the **least stable** across the 3 folds.

### Step 4: Dealing with the 8% imbalance (4 tricks tested on LightGBM)
With only 8% bad clients, a model can get lazy and predict "repays" for everyone. Four ways to fight that:

| Trick | Plain words | Analogy |
|---|---|---|
| **Nothing** | Train as is | |
| **Class weights** | A mistake on a bad client counts ~11× more during training | Exam where the hard questions are worth more points |
| **Undersampling** | Randomly throw away good clients until both groups are the same size | Fewer but balanced examples |
| **SMOTE** | Create **fake** bad clients by blending real ones that look alike | Inventing extra practice questions similar to the real hard ones |

### The results table

| Model | Imbalance trick | AUC (higher = better) | Cost at best threshold (lower = better) | Cost at 50% |
|---|---|---|---|---|
| Dummy | none | 0.500 | 0.807 | 0.807 |
| Logistic regression | class weights | 0.769 | 0.511 | 0.513 |
| Random forest | class weights | 0.746 | 0.544 | 0.570 |
| Neural network | undersampling | 0.741 | 0.551 | 0.551 |
| XGBoost | weights | 0.780 | 0.499 | 0.505 |
| LightGBM | none | 0.779 | 0.500 | **0.778** ⚠️ |
| **LightGBM** | **class weights** | **0.780** | **0.498** 🏆 | 0.498 |
| LightGBM | undersampling | 0.775 | 0.505 | 0.506 |
| LightGBM | SMOTE | 0.775 | 0.507 | 0.790 ⚠️ |

**How to read it with a beginner:** look at the ⚠️ rows. LightGBM with no trick is a *good* model (AUC 0.779), but at the default 50% threshold it costs 0.778, almost as bad as the dummy. Why? Its probabilities are all low (8% world), so almost nobody goes above 50%, and it accepts nearly every bad client. Move the threshold to 8% and the cost drops to 0.500.

**Lesson #1: choosing the threshold matters more than which trick you use.** Once the threshold is optimised, all 4 tricks land between 0.498 and 0.507.

**Winner: LightGBM with class weights.** It has the lowest cost, it is fast (98 s vs 195 s for XGBoost), and it needs no extra preparation steps.

### Hyperparameters: the "knobs" of each model

**Parameters vs hyperparameters:**
- **Parameters** are learned *by the model* during training (the scorecard points of logistic regression, the questions inside each tree).
- **Hyperparameters** are set *by us* before training, like the settings of a washing machine: temperature, spin speed, duration.

**The one idea that explains almost every knob: too complex vs too simple.**

```
too simple  ◄──────────────── sweet spot ────────────────►  too complex
"underfitting"                                           "overfitting"
misses real patterns                         memorises noise in the training data
(bad everywhere)                             (great in training, bad on new clients)
```

Most hyperparameters are a dial between these two ends. Notice that most of our choices push towards *simpler*: with 768 columns and 8% defaults, overfitting is the bigger danger.

#### 🎲 Dummy: `DummyClassifier(strategy="prior")`
| Knob | Value | What it does |
|---|---|---|
| `strategy` | `"prior"` | Always predict the share of defaults seen in training (8%). The only knob. |

#### 📏 Logistic regression (a 3-step pipeline)
| Step | Knob | Value | What it does |
|---|---|---|---|
| Imputer | `strategy` | `"median"` | Fills each empty cell with the median of its column, learned on training data only |
| Scaler | (none) | | Rescales each column to mean 0 and spread 1, so income in millions doesn't overpower a 0–1 score |
| Model | `class_weight` | `"balanced"` | A mistake on a defaulter counts ~11× more (because there are ~11 good clients per bad one) |
| Model | `max_iter` | `1000` | The model finds its points by trial and error, step by step. This is the maximum number of steps. The default of 100 isn't enough with 768 columns. |
| Model | `C` | `1.0` (default) | **Regularisation**: stops the model from giving huge points to any one column. Smaller C = more cautious. |

#### 🌲🌲🌲 Random forest
| Knob | Value | What it does | Analogy |
|---|---|---|---|
| `n_estimators` | `200` | Number of trees | Number of people in the vote. More = more stable, but slower |
| `max_depth` | `12` | Maximum number of questions in a row in one tree | Max levels in "20 Questions". Deeper = more detailed = more risk of memorising |
| `min_samples_leaf` | `50` | Each final answer box must contain at least 50 clients | Forbids conclusions drawn from only 2 or 3 people |
| `max_features` | `"sqrt"` | At each question, a tree may only look at √768 ≈ 28 random columns | Forces trees to be different from each other, which makes the vote useful |
| `class_weight` | `"balanced_subsample"` | Same as "balanced", recomputed for each tree's random sample | |
| `n_jobs` | `-1` | Use all processor cores | Practical only, no effect on results |
| `random_state` | `42` | Fixes the randomness | Same result every time you run it (reproducibility) |

#### 🚀 XGBoost
| Knob | Value | What it does | Analogy |
|---|---|---|---|
| `n_estimators` | `300` | Number of trees built one after another | Number of correction rounds |
| `learning_rate` | `0.05` | How much of each tree's correction is kept (5%) | Small, careful steps. Smaller = more trees needed, but usually better |
| `max_depth` | `6` | Max depth of each tree | Boosting trees are kept **shallow** on purpose: each one only fixes a small part |
| `subsample` | `0.8` | Each tree trains on a random 80% of clients | Adds randomness → less memorising |
| `colsample_bytree` | `0.5` | Each tree sees a random 50% of columns | Same idea, for columns |
| `scale_pos_weight` | `11.39` | Errors on defaulters count 11.39× more | XGBoost's version of `class_weight` (= 282k good ÷ 25k bad) |
| `tree_method` | `"hist"` | Groups values into bins before searching for splits | Much faster, almost the same quality |

**The `learning_rate` × `n_estimators` trade-off:** imagine walking to a target. Big steps (high rate) get you close quickly but overshoot. Small steps (low rate) are precise but need many more steps (trees). These two knobs are always tuned together.

#### 🧠 Neural network (MLP, 4-step pipeline)
| Knob | Value | What it does |
|---|---|---|
| Imputer + Scaler | median, standard | Same as logistic regression. Neural networks need full, rescaled data |
| `RandomUnderSampler` | default | Throws away good clients at random until both groups are the same size (~20k each) |
| `hidden_layer_sizes` | `(64, 32)` | 2 hidden layers: 64 neurons, then 32. More/larger = can learn more complex patterns, but overfits more easily |
| `early_stopping` | `True` | Keeps 10% of its training data aside and **stops as soon as the score on it stops improving** |
| `max_iter` | `100` | Maximum number of passes over the training data (called "epochs") |
| `activation`, `solver` | `relu`, `adam` (defaults) | How each neuron transforms its input / the method used to adjust the weights. Standard choices; no need to go deeper with a beginner |

**Early stopping analogy:** a student revising for an exam. At first, practice scores improve. After a while, they're only memorising the practice book and their real level stops improving. Early stopping says "stop revising now."

#### 🚀 LightGBM: default vs tuned
LightGBM has the same family of knobs as XGBoost (same idea, different names). The comparison runs used the **defaults**. Optuna then searched for better values.

| Knob | Default | Optuna's choice | What it does | What the change means |
|---|---|---|---|---|
| `n_estimators` | 100 | **350** | Number of trees | More trees… |
| `learning_rate` | 0.1 | **0.032** | Size of each correction | …each with a smaller step. The classic "slow and careful" combination |
| `num_leaves` | 31 | **23** | Max number of final answer boxes per tree (LightGBM's main complexity knob) | Simpler trees |
| `max_depth` | -1 (no limit) | **8** | Max depth of each tree | A safety limit on top of `num_leaves` |
| `min_child_samples` | 20 | **194** | Min clients in each final box | **Much more cautious**: no conclusion from fewer than 194 clients |
| `subsample` | 1.0 | **0.87** | Share of clients used per tree | A little randomness |
| `colsample_bytree` | 1.0 | **0.30** | Share of columns per tree | Each tree sees only ~230 of 768 columns, so trees can't all rely on the same few |
| `reg_lambda` | 0 | **0.039** | Penalty on extreme answers in the boxes | A very light brake |
| `class_weight` | | `"balanced"` | Errors on defaulters count ~11× more | Kept from the comparison |
| `subsample_freq` | 0 | `1` | Re-draw the random clients at every tree | Needed, otherwise LightGBM ignores `subsample` |

**How to summarise Optuna's choice for a beginner:** *"many small, cautious trees, each looking at a different part of the data."* Every change goes in the direction of **less overfitting**. That makes sense with 768 columns and only 8% defaulters.

**A detail for a curious student:** `min_child_samples = 194` is right next to the top of the search range (200). Optuna might have gone higher if allowed. Widening that range is a cheap possible improvement.

#### ⚖️ Imbalance tools
| Tool | Knob | Value | What it does |
|---|---|---|---|
| `RandomUnderSampler` | `random_state` | 42 | Which good clients get thrown away is random but reproducible |
| `SMOTE` | `k_neighbors` | 5 (default) | Each fake defaulter is created between a real defaulter and one of its 5 most similar defaulters |

**Important:** these are applied **only during training**, never when predicting. You can't invent or throw away real clients who apply for a loan.

#### 🔁 Knobs shared by the whole notebook
| Knob | Value | What it does |
|---|---|---|
| `RANDOM_STATE` | 42 | Same random choices every run, so results are reproducible (42 is just a programmer joke; any number works) |
| `StratifiedKFold(n_splits=3, shuffle=True)` | 3 folds | 3 mock exams, each with 8% defaulters |
| `test_size` | 0.2 | 20% of clients kept for the final exam |
| `FN_COST`, `FP_COST` | 10, 1 | Not a model knob but a **business** assumption. Changing it changes the threshold |

### Step 5: Fine-tuning with Optuna
- **Hyperparameters** = the model's settings, chosen by *us* before training: number of trees, how big each tree can be, how fast it learns… (Different from what the model learns by itself.)
- **Analogy:** tuning an oven: temperature, time, rack position.
- Trying every combination (**grid search**) = 6,561 combinations × 3 folds: way too long.
- **Optuna** is smarter: it tries a setting, looks at the result, and tries the next one **near the good ones**. 15 tries were enough.
- Goal given to Optuna: **minimise the business cost**.
- **The "Optimization History Plot"** (the chart right after the Optuna cell):
  - x-axis = trial number (0 to 14), y-axis = business cost (lower = better)
  - **each dot** = one try, with its score
  - **the line** = the best score found *so far*. It can only go down or stay flat.
  - Our run: trial 0 = 0.524, the first big drop at trial 2 (0.502), then 0.499 at trial 5, and the best at trial 12 (0.498). Dots keep jumping up (trials 3, 7, 9) because Optuna still explores new areas sometimes. The line goes flat at the end, so more trials would probably gain very little.
  - Analogy: hunting for the lowest point of a valley in fog. Each step tells you the altitude, and you keep a note of the lowest spot seen so far.
- Result: cost improved from 0.498 to 0.492. A small gain: LightGBM's default settings were already good.

### Step 6: The final exam
- Threshold chosen on the cross-validation predictions: **0.49**. Checked on each fold separately (0.48 / 0.51 / 0.52): stable.
- The cost-vs-threshold chart: a "U" shape. Too low → refuse too many good clients. Too high → accept too many bad ones. Best = bottom of the U.
- Why is it close to 50% here? Because class weights already pushed the probabilities up. Without them it was 8%. **The threshold always goes together with the training choices.**
- **Validation results (the exam):** AUC **0.786**, cost **0.489**, catches **71%** of bad clients (recall).
- Mock exams (CV) said 0.784 and the real exam says 0.786: the model **didn't just memorise**, it generalises.

**"Only 18% precision, isn't that bad?"** It means that out of 100 refused clients, 18 would really have defaulted. But each missed bad client costs 10× more than each wrongly refused good one, so refusing a bit too many is the cheaper mistake.

### Step 7: MLflow (the lab notebook)
- Every experiment (9 comparisons, 15 Optuna tries, the final model) is automatically recorded: settings, scores, charts.
- **Analogy:** a scientist's lab notebook, but automatic and searchable.
- The final model is saved in a **registry** under the name `credit_scoring_lgbm` with the label **`champion`**, plus its threshold (0.49). An app can load "the champion" without knowing anything about this notebook. The last cell does exactly that on 5 new clients.
- To show it live (run from the project root, then open http://localhost:5000):

```bash
mlflow ui --backend-store-uri sqlite:///mlflow.db
```

### Questions to ask the student
1. Random forest vs boosting: what is the difference? *(Parallel voting vs sequential correction.)*
2. Why did LightGBM-without-tricks cost so much at 50%, yet so little at 8%?
3. Why must the validation set stay locked until the end?
4. If a missed default cost 5× instead of 10×, would the threshold go up or down? *(Up: missing bad clients hurts less, so we can accept more.)*

---

## 4. Notebook 04: Interpretability ("why this decision?")

**One sentence:** *"A refusal must be explainable: to the client, to the manager, and to check the model isn't unfair."*

### Two levels
- **Global:** "In general, what does the model look at most?"
- **Local:** "Why was **this** client refused?"

### Method 1: LightGBM's own importance (global only)
Counts how much each column helped the trees overall.
Top: `EXT_SOURCE_2`, `EXT_SOURCE_3`, `EXT_SOURCE_1`, `PAYMENT_RATE`, `DAYS_EMPLOYED`, `DAYS_BIRTH`, `CODE_GENDER`…
**Limit:** it says *how much* a column matters, not *in which direction* (more risk or less?), and it can't explain one client.

### Method 2: SHAP (global **and** local)
**The idea, for a beginner:** the model's answer is shared out between the features, like splitting a restaurant bill fairly between friends according to what each one ate.

```
client's score = average score + contribution of feature 1 + contribution of feature 2 + ...
```

- A **positive** contribution (red) pushes towards **refusal**.
- A **negative** contribution (blue) pushes towards **acceptance**.
- Units are "log-odds", the model's internal scale. Just read the **sign** and the **size**.
- **Proof it works (cell 11):** the SHAP sum gives exactly the model's probabilities: 0.674 / 0.077 / 0.480 both ways. ✅

### What the results show (from the run)

**Global ranking (SHAP, top 10):** 1. `EXT_SOURCE_2` · 2. `EXT_SOURCE_3` · 3. `EXT_SOURCE_1` · 4. `CODE_GENDER` · 5. `DAYS_EMPLOYED` · 6. `PAYMENT_RATE` · 7. `APPROVED_CNT_PAYMENT_MEAN` · 8. `AMT_ANNUITY` · 9. `INSTAL_DPD_MEAN` · 10. `OWN_CAR_AGE`

**Reading the beeswarm chart** (1 dot = 1 client; right = more risk; red = high value, blue = low value):

| Feature | What the chart shows | In plain words |
|---|---|---|
| `EXT_SOURCE_1/2/3` | red on the left, blue far right | **Good external credit score → lower risk.** Biggest effect of all. |
| `CODE_GENDER` (0 = man, 1 = woman) | red (women) left, blue (men) right | **Being a man increases the predicted risk** (see fairness below) |
| `DAYS_EMPLOYED` | red on the right | Short time in current job → more risk |
| `PAYMENT_RATE` | red dots far left, but low values can also be on the left | **Not a straight line:** the accepted client has a *very low* rate (0.025) and it lowers their risk, while the refused client's middle value (0.081) raises it. Tree models can learn these curved effects; logistic regression can't. One of the ratios created in notebook 02! |
| `APPROVED_CNT_PAYMENT_MEAN` | red on the right | Past loans with many installments (long loans) → more risk |
| `INSTAL_DPD_MEAN` | red on the right | Was late on past payments → more risk |
| `DAYS_BIRTH` | red on the right | Younger → more risk (matches the age chart from notebook 01) |
| Married / Higher education | red on the left | Married or degree-holders → less risk |

**This is a great moment for the student:** almost everything we guessed in notebook 01 (external scores, age, job stability) is confirmed by the model. And features we *created* in notebook 02 (`PAYMENT_RATE`, `INSTAL_DPD_MEAN`, `APPROVED_…`) are in the top 10. The work was worth it.

### Two real clients (waterfall charts)
Read a waterfall from the bottom (average client, -0.53) to the top (this client's final score).

**Refused client (209192): probability 67%, above the 0.49 threshold**
- ➕ `EXT_SOURCE_2` = 0.32 (low score): **biggest push towards refusal**
- ➖ `EXT_SOURCE_3` = 0.59 (decent score): pulls back towards acceptance
- ➕ man, ➕ shortish job (3 years), ➕ age ~34, ➕ payment rate
- ➖ occupation "Core staff"
- **Teaching point:** the "757 other features" bar is **+0.81**, bigger than any single feature. No single reason caused the refusal: many small signals added up. This is honest, and important when explaining a refusal to someone.

**Accepted client (267442): probability 8%**
- ➖ very good `EXT_SOURCE_2` (0.77) and `EXT_SOURCE_3` (0.72)
- ➖ long loan / low payment rate, ➖ 33 years in the same job, ➖ married
- ➕ man (+0.11), ➕ `EXT_SOURCE_1` missing (+0.06): small pushes up, not enough to change the decision.

The `explain_client()` table gives the same thing as text. That is what an app would send back with each decision.

### ⚖️ Fairness: the big discussion point
`CODE_GENDER` is the **4th most important feature** (SHAP) and the 7th (LightGBM gain). Being a man increases the risk score, by +0.18 for the refused client. Both example clients are men.
- Is it "true" in the data? Yes (10.1% vs 7.0% default, notebook 01).
- Is it **acceptable** to use? In many countries, deciding credit based on gender is **illegal discrimination**.
- **Next step proposed in the notebook:** retrain without `CODE_GENDER` and compare the cost. If the cost barely changes, remove it.
- Good question for the student: *"If we remove gender, can the model still guess it from other columns?"* (Yes, partly, through proxies like occupation. Fairness is harder than just deleting one column.)

### Questions to ask the student
1. What's the difference between global and local importance?
2. On the beeswarm, how do you see that a *high* external score *lowers* the risk?
3. Why is "757 other features: +0.81" an important thing to show a manager?
4. Should we keep gender in the model? Why or why not?

---

## 5. Suggested 1-hour session plan

| Time | Part | Focus |
|---|---|---|
| 0–5 min | Story (section 0) | The problem + "learning from examples" |
| 5–15 min | Notebook 01 | 8% imbalance, age chart, the 365243 mystery |
| 15–25 min | Notebook 02 | Whiteboard "summarise, then attach" + banker ratios |
| 25–45 min | Notebook 03 | Models explained (tree → forest vs boosting), cost table, threshold |
| 45–55 min | Notebook 04 | Beeswarm + the two clients + gender debate |
| 55–60 min | Questions | Pick 3–4 from the lists above |

**Don't run notebook 03 live** (it takes about 40 minutes). Show the saved outputs and the MLflow interface.

---

## 6. Things still to fix in the notebooks

1. ✅ **Notebook 03, "My choice" paragraph:** the timing now matches the outputs (98 s vs 195 s for XGBoost).
2. ✅ The fairness claim in notebook 04 (`CODE_GENDER` among the top features, men = higher risk) is **confirmed** by the run: rank 4 by SHAP.
3. Notebook 04's comparison table (cell 17) prints the SHAP and gain lists side by side. Gain ranks `PAYMENT_RATE` 4th and `CODE_GENDER` 7th. Worth one sentence in the markdown.
4. One thing that may confuse students: the "average prediction" in SHAP is -0.528 log-odds ≈ **37%**, not 8%. That's because class weights inflate the probabilities. Mention it if someone asks.

---

## Glossary

| Word | Plain meaning |
|---|---|
| Model | A recipe learned from examples that turns a client's info into a probability |
| Feature | A column the model uses (income, age…) |
| Target | What we want to predict (defaulted: yes/no) |
| Training / validation set | Study material / final exam the model never saw |
| Cross-validation | Several mock exams, rotating which part is used for testing |
| Overfitting | Memorising the training examples instead of learning general patterns |
| Threshold | The probability above which we refuse |
| False positive / negative | Refused a good client / accepted a bad client |
| Recall | Share of bad clients we caught |
| Precision | Share of refused clients who really were bad |
| ROC AUC | Chance that the model ranks a random bad client above a random good one |
| Imbalance | One class (defaults) is much rarer than the other |
| Encoding | Turning text categories into numbers |
| Aggregation | Summarising many rows into one (mean, max, count…) |
| Data leakage | The model accidentally seeing information it won't have in real life |
| Hyperparameter | A setting we choose before training (number of trees…) |
| Parameter | What the model learns by itself during training |
| Underfitting | Model too simple: misses real patterns |
| Learning rate | How big each correction step is in boosting |
| Regularisation | A brake that stops the model from relying too much on any one thing |
| Early stopping | Stop training when the score on held-back data stops improving |
| EXT_SOURCE | Credit scores (0–1) bought from undisclosed outside sources; higher = safer |
| Bagging / Boosting | Many models in parallel voting / in sequence fixing each other's mistakes |
| SHAP value | How much one feature pushed one client's score up or down |
| MLflow | Automatic lab notebook + storage for models |

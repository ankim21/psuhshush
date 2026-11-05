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
- **Data leakage** = letting the model "peek" at information it won't have in real life, which makes it look better than it is. Mixing train and test here is OK because every step works client by client and never looks at TARGET. Steps that *learn* from the data (like computing the median) are done later, on the training data only.
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

### Step 5: Fine-tuning with Optuna
- **Hyperparameters** = the model's settings, chosen by *us* before training: number of trees, how big each tree can be, how fast it learns… (Different from what the model learns by itself.)
- **Analogy:** tuning an oven: temperature, time, rack position.
- Trying every combination (**grid search**) = 6,561 combinations × 3 folds: way too long.
- **Optuna** is smarter: it tries a setting, looks at the result, and tries the next one **near the good ones**. 15 tries were enough.
- Goal given to Optuna: **minimise the business cost**.
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

1. **Notebook 03, "My choice" paragraph:** it says "54 seconds against 122 for XGBoost", but the outputs show **98 s vs 195 s**. The conclusion still holds; only the numbers are wrong.
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
| Bagging / Boosting | Many models in parallel voting / in sequence fixing each other's mistakes |
| SHAP value | How much one feature pushed one client's score up or down |
| MLflow | Automatic lab notebook + storage for models |

# SuperMarket Manager (C, Data Structures & Files)

Console-based **C** project that models a supermarket back-office: products, customers, carts, members, sorting & search, and persistent storage. The focus is on **OOP-style design in C** (via structs & function pointers), a **generic singly linked list** (`void*` payload), **binary/text file I/O**, and **library algorithms** (`qsort`, `bsearch`). Built and tested on **Visual Studio (Windows)**.

---

## ✨ Key Capabilities

- **Generic Linked List (`void*`)**
  - `ListInit`, `ListInsertSorted`, `ListRemove`, `ListFree`, `ListPrint`.
  - Nodes store **`void*` values**; deletions support both ownership modes:
    - free **node only**, or free **node + value** via callback (prevents leaks/double-free).
  - Used for the **ShoppingCart** instead of arrays; items kept **sorted on insert** by **barcode** (no post-sort, no aux structures).

- **Customers & Club Members (Polymorphism in C)**
  - `Customer` base struct; **`ClubMember` “inherits”** via first-field embedding.
  - Discount rules:
    - `≤ 24 months`: **0.1% per month** (max 2.3%).
    - `2–5 years`: **2.5% + 0.5% per year** (max 4.5%).
    - `> 5 years`: **7.5%** flat.
  - `initCustomer()` asks if member and reads tenure; `printCustomer()` shows membership & months.
  - Other APIs adjusted for correct **polymorphic behavior** (function pointers where relevant).

- **Products: Sort & Search**
  - Products array may be sorted by **Name / Quantity / Price** via **`qsort`**.
  - Track current order with an **`enum`** (includes “unsorted”).
  - **`bsearch`** is enabled only **when array is sorted by the same key** (otherwise report “cannot search”).

- **Function Pointers Utility**
  - `generalArrayFunction(void* base, size_t n, size_t elemSize, void (*f)(void*))`  
    Iterates elements and calls `f`—used by `printAllProducts()` and can be reused for validators/serializers.

- **Persistent Storage**
  - **Binary file**: `SuperMarket.bin` stores everything **except the customers list**.
    - Strings saved as: **length including '\0'**, then the bytes (safer than raw pointers).
    - Product record: *name*, *barcode*, *type*, *price*, *quantity*, *expiry date*.
    - Sort mode is **not** saved; on load the array is considered **unsorted**.
  - **Text file**: `Customers.txt` stores customers.
    - Format: first **count**, then per customer: *name*, *ID*, *member? + tenure (months)*.

- **Program Lifecycle**
  - **Startup**: attempt to **load** supermarket from files; on failure, **initialize from user**.
  - **Menu** additions:
    - **Sort** products (sub-menu by enum).
    - **Search** product according to current sorted key.
  - **Exit**: **save** supermarket data to `SuperMarket.bin` and `Customers.txt`.

---

## 🧱 Design & Architecture

- **Encapsulation & SRP**
  - Each module owns its data & operations: `Product`, `Customer/ClubMember`, `ShoppingCart` (list wrapper), `SuperMarket` (owning arrays & IO), `List` (generic).
  - UI code (**`main`**) contains only menu/input/printing and delegates to modules.

- **Polymorphism in C**
  - “Inheritance” by embedding: `typedef struct { Customer base; int totalMonths; } ClubMember;`
  - Virtual-like behavior via **function pointers** (e.g., price calculation with member discount).
  - All code paths accept **`Customer*`** where possible; when needed, detect `ClubMember` safely.

- **Linked List Insert-sorted**
  - O(N) insert that keeps **stable order by barcode** and avoids extra storage.
  - Centralized **comparator** isolates ordering logic from list mechanics.

- **Memory Safety**
  - Clear ownership rules (e.g., cart nodes own item clones vs. references).
  - List free accepts a **destructor callback** to release `void*` payloads when appropriate.
  - Defensive checks for allocations, file handles, and indexes.

- **File I/O Robustness**
  - Binary serialization with **length-prefixed strings** eliminates pointer & delimiter pitfalls.
  - Text format for customers is human-readable and easy to diff.
  - On load, validate counts, lengths, and field ranges; on error, fallback to interactive init.

- **Algorithms**
  - `qsort` comparators for **Name/Quantity/Price**; **`bsearch`** uses the matching comparator only when legal.
  - `generalArrayFunction` abstracts iteration; easy to reuse (print, sanity checks, zeroing, etc.).

---

## 🧪 Complex Problems Solved

- **Generic List with Mixed Ownership**
  - Problem: some payloads must be freed with the node, others must not.  
  - Solution: **destructor callback** parameter on remove/free; default `NULL` skips freeing the payload.

- **Sorted Cart without Auxiliary Arrays**
  - Problem: keep `ShoppingItem`s **always ordered by barcode** without sorting the whole list.
  - Solution: **insert-sorted** into a singly linked list using a comparator; O(N) insert, O(1) for head/tail.

- **Polymorphic Discounts in C**
  - Problem: express **member vs. non-member** pricing in C (no classes).  
  - Solution: first-field embedding + **function pointer table** (or conditional) for price computation & printing.

- **Safe Persistence**
  - Problem: saving **dynamic strings** & complex records without pointers.  
  - Solution: **length-prefixed** encoding; strict reading with size checks; separate binary (products) vs text (customers).

- **Search Correctness**
  - Problem: `bsearch` requires a **sorted array** by the **same key**.  
  - Solution: track current order via an `enum`; **refuse search** when unsorted or key mismatch (prevents undefined results).

---

## 🖥️ Build & Run (Visual Studio)

1. Open the solution in Visual Studio (Windows).
2. Build (no warnings).
3. Run. On first run, if files are missing/corrupt, initialize data via the console prompts.

> The assignment required Visual Studio compatibility, short functions, no duplicate code, strong input checks, and exact output formatting; the project follows these guidelines.

---

## 📂 Files & Formats

- **Binary** `SuperMarket.bin`  
  - [Shop name] → [products count] → repeated **Product** blocks:  
    `[name(len+1,str)] [barcode] [type] [price] [quantity] [expiry]`
- **Text** `Customers.txt`  
  - `[customers count]` then, for each: `[name] [ID] [member? + months]`

> At startup, sort mode is assumed **unsorted** regardless of previous runs (not persisted).

---

## 🧠 What I Learned

- **C data-modeling** that mimics OOP: composition, “inheritance” via embedding, and **polymorphism** with function pointers.  
- **Generic containers** in C with `void*` + safe destruction policies.  
- **Insert-sorted linked lists** for always-sorted views without global sorts.  
- **Algorithmic APIs** (`qsort`, `bsearch`) and writing **reusable comparators**.  
- **Binary vs. text persistence**, designing **stable on-disk formats**, and robust error handling.  
- **Defensive coding**: bounds checks, allocation checks, ownership clarity, and small functions (≤ 25 lines) per guideline.

---

## 🗺️ Roadmap

- Replace product arrays with dynamic vectors; add **search by prefix**, **range queries**, and **stats**.  
- Persist customers in binary too (with versioning).  
- Unit tests for comparators, serializers, and list operations.  
- CLI polishing and import/export CSV.

---



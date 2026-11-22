/*
CS 211 Bank of Antarctica
Name:Touthy Mienphilom 

PART D Written explanations
Replace each answer placeholder with a short paragraph in this comment.
1. Repeated overdrafts: The starter code used = instead of += when updating a customer's loan. Because it used =, a second overdraft erased whatever debt the customer already owed and replaced it with only the new overdraft and $35.00 fee. 
    Switching to += adds the new shortfall and fee on top of any existing debt so previous unpaid loans are not lost.
2. Addresses and values: Writing balances + account_id gives the memory address (a double* pointer) of that customer's balance, while writing balances[account_id] gives the actual dollar amount (a double value) stored there. 
    The starter code tried to store the dollar value inside a pointer variable, which caused a compiler error. Using balances + account_id stores the address properly so we can change the customer's real balance in the array.
3. Recursive search: The recursive search uses two functions: IsSubString steps through each letter of name as a possible starting spot, and a helper function checks if search matches letter-by-letter from that spot. 
    The recursion stops and returns true if search reaches '\0' (all letters matched), or returns false if name reaches '\0' first. Checking every starting letter one by one makes sure overlapping patterns like "aba" in "ababa" are always found.
4. Independent snapshots: Setting snapshot = current only copies the memory address, meaning both pointers point to the exact same array. If current changes later, snapshot would change right along with it instead of keeping the old numbers. 
    Using new double[count] creates a brand-new array in memory and copies each number over, so later edits to current do not change the saved snapshot.
5. Memory ownership: When copy_balances creates an array with new double[count], the caller becomes the owner of that memory and must clean it up when finished. Just setting snapshot = nullptr causes a memory leak because the program loses the address of the array without freeing it. 
    Calling delete[] snapshot properly frees the memory, and passing the pointer by reference (double*&) lets release_snapshot set the caller's pointer to nullptr so it is not left pointing to deleted memory.
*/

#include <iostream>
#include <iomanip>
using namespace std;

const int NAME_WIDTH = 100;
const int CUSTOMER_COUNT = 11;

// ===== PART A Bank repairs =====
// PART A: Repair the Part A functions using the assignment's banking rules.
// Keep the function names, parameter lists, and return types unchanged.
// The errors are intentional. Some prevent compilation; others need testing.

void deposit_to_account(double balances[], double loans[], int account_id,
                        double amount) {
    if (amount <= 0.0) return;

    double* selected_balance = balances + account_id;
    double* selected_loan = loans + account_id;

    if (*selected_loan > 0.0) {
        if (amount < *selected_loan) {
            *selected_loan -= amount;
        } else {
            double remaining = amount - *selected_loan;
            *selected_loan = 0.0;
            *selected_balance += remaining;
        }
    } else {
        *selected_balance += amount;
    }
}

void withdraw_from_account(double balances[], double loans[], int account_id,
                           double amount) {
    if (amount <= 0.0) return;

    double* selected_balance = balances + account_id;
    double* selected_loan = loans + account_id;

    *selected_balance -= amount;

    if (*selected_balance < 0.0) {
        *selected_loan += -*selected_balance + 35.0;
        *selected_balance = 0.0;
    }
}

void refresh_snapshot(double*& snapshot, const double* source, int count) {
    // The caller owns snapshot. On entry it is null or owns a new[] array.
    // Replace it with a complete, independent copy of source.
    delete[] snapshot;
    if (count == 0) {
        snapshot = nullptr;
        return;
    }

    double* replacement = new double[count]();
    for (int i = 0; i < count; ++i) {
        *(replacement + i) = *(source + i);
    }
    snapshot = replacement;
}

// ===== PART B Recursive substring search =====
// PART B: This old iterative algorithm is intentionally incomplete.
// Replace it with a correct recursive implementation. You may add helpers.
// The public function signature must stay unchanged.
bool matches_prefix(const char* name, const char* search) {
    if (*search == '\0') return true;
    if (*name == '\0') return false;
    if (*name != *search) return false;
    return matches_prefix(name + 1, search + 1);
}

bool IsSubString(const char* name, const char* search) {
    if (*search == '\0') return true;
    if (*name == '\0') return false;
    if (matches_prefix(name, search)) return true;
    return IsSubString(name + 1, search);
}

// ===== PART C Pointer snapshot functions =====
// PART C: Replace these compiling placeholders with your implementation.
// Use new[], delete[], pointer arithmetic, and dereferencing as specified.
double* copy_balances(const double* source, int count) {
    if (count <= 0 || source == nullptr) {
        return nullptr;
    }

    double* snapshot = new double[count];
    for (int i = 0; i < count; ++i) {
        *(snapshot + i) = *(source + i);
    }

    return snapshot;
}

void release_snapshot(double*& snapshot) {
    delete[] snapshot;
    snapshot = nullptr;
}

// ===== PROVIDED SEARCH AND INPUT HELPERS Keep unchanged =====

// Provided working support for Part A. Keep this working helper unchanged.
bool contains_iterative(const char* name, const char* search) {
    if (*search == '\0') return true;
    for (const char* start = name; *start != '\0'; ++start) {
        const char* text = start;
        const char* query = search;
        while (*text != '\0' && *query != '\0' && *text == *query) {
            ++text;
            ++query;
        }
        if (*query == '\0') return true;
    }
    return false;
}

bool read_number(double& value) {
    // A failed numeric read ends the session; input recovery is not required.
    if (!(cin >> value)) return false;

    // >> leaves the line ending behind. Read to the end of this line so
    // the next getline starts with the customer's search, not an old newline.
    char ch;
    while (cin.get(ch) && ch != '\n') {
        // Consume the remaining characters on this numeric-input line.
    }
    return true;
}

// ===== PART A continued Repair customer selection =====
int select_user(const char names[][NAME_WIDTH], int count) {
    char search[NAME_WIDTH] = {};
    while (true) {
        int matches = 0;
        int selected = -1;

        cout << "Search by part of a name: ";
        if (!cin.getline(search, NAME_WIDTH)) return -1;
        // An empty search must be rejected here, even though a general
        // substring function considers an empty search to be a match.
        if (search[0] == '\0') {
            cout << "Enter at least one character.\n";
            continue;
        }
        for (int i = 0; i < count; ++i) {
            if (contains_iterative(names[i], search)) {
                ++matches;
                selected = i;
                cout << "Match: " << names[i] << '\n';
            }
        }
        if (matches == 1) return selected;
        cout << "Enter a search matching exactly one customer.\n";
    }
}

// PROVIDED OUTPUT HELPER Keep the record format unchanged.
void print_accounts(const char names[][NAME_WIDTH], const double balances[],
                    const double loans[], int count) {
    cout << fixed << setprecision(2);
    for (int i = 0; i < count; ++i) {
        cout << "ACCOUNT|" << i << '|' << names[i] << '|'
             << balances[i] << '|' << loans[i] << '\n';
    }
}
// PROVIDED DRIVER: menu option 4 tests your Part B function.
bool run_search_test() {
    char name[NAME_WIDTH] = {};
    char search[NAME_WIDTH] = {};
    cout << "Name: ";
    if (!cin.getline(name, NAME_WIDTH)) return false;
    cout << "Search: ";
    if (!cin.getline(search, NAME_WIDTH)) return false;
    cout << "Result: " << (IsSubString(name, search) ? "true" : "false") << '\n';
    return true;
}

// PROVIDED DRIVER: menu option 5 tests your Part C functions.
// This local array is separate from the customers' real accounts.
bool run_snapshot_test() {
    double entered_count;
    cout << "Number of balances from 0 to 99: ";
    if (!read_number(entered_count)) return false;
    if (entered_count < 0 || entered_count > 99) {
        cout << "Choose a count from 0 to 99.\n";
        return true;
    }
    int count = static_cast<int>(entered_count);
    if (count != entered_count) {
        cout << "The count must be a whole number.\n";
        return true;
    }
    double current[100] = {};
    for (int i = 0; i < count; ++i) {
        cout << "Balance " << i << ": ";
        if (!read_number(current[i])) return false;
    }
    double change = 0.0;
    if (count > 0) {
        cout << "Increase first balance by: ";
        if (!read_number(change)) return false;
    }
    double* saved = copy_balances(count == 0 ? nullptr : current, count);
    if (count == 0) {
        cout << "Empty snapshot: " << (saved == nullptr ? "true" : "false") << '\n';
        release_snapshot(saved);
        cout << "Released and reset: " << (saved == nullptr ? "true" : "false") << '\n';
        return true;
    }
    if (saved == nullptr) {
        cout << "copy_balances has not produced a snapshot yet.\n";
        return true;
    }
    if (saved == current) {
        cout << "Snapshot aliases current balances; an independent allocation is required.\n";
        return true;
    }
    current[0] += change;
    cout << fixed << setprecision(2);
    for (int i = 0; i < count; ++i) {
        cout << "Saved[" << i << "]: " << saved[i]
             << " | Current[" << i << "]: " << current[i] << '\n';
    }
    release_snapshot(saved);
    cout << "Released and reset: " << (saved == nullptr ? "true" : "false") << '\n';
    return true;
}

// ===== PART A continued Repair main transaction handling and cleanup =====

int main() {
    const char names[CUSTOMER_COUNT][NAME_WIDTH] = {
        "john fitzgerald kennedy", "william shakespeare",
        "salvador felipe jacinto dali domenech", "elvis aaron presley",
        "elizabeth alexandra mary windsor", "marie curie", "pierre curie",
        "john lennon", "anna pavlova", "alan turing", "ada lovelace"
    };
    double balances[CUSTOMER_COUNT] = {
        100.50, 120.75, 50.20, 150.15, 200.99, 80.00,
        90.00, 60.00, 70.00, 110.00, 130.00
    };
    double loans[CUSTOMER_COUNT] = {};
    double* before = nullptr;

    cout << "BANK OF ANTARCTICA\n";
    print_accounts(names, balances, loans, CUSTOMER_COUNT);
    while (true) {
        cout << "0 Exit | 1 Show accounts | 2 Deposit | 3 Withdraw | 4 Search test | 5 Snapshot test\n";
        double choice;
        if (!read_number(choice)) break;
        if (choice == 0.0) break;
        if (choice == 1.0) {
            print_accounts(names, balances, loans, CUSTOMER_COUNT);
            continue;
        }
        if (choice == 4.0) {
            if (!run_search_test()) break;
            continue;
        }
        if (choice == 5.0) {
            if (!run_snapshot_test()) break;
            continue;
        }
        if (choice != 2.0 && choice != 3.0) {
            cout << "Choose a menu option from 0 to 5.\n";
            continue;
        }

        int id = select_user(names, CUSTOMER_COUNT);
        if (id < 0) break;
        cout << "Amount: ";
        double entered;
        if (!read_number(entered)) break;
        double amount = entered;

        refresh_snapshot(before, balances, CUSTOMER_COUNT);
        if (choice == 2.0) {
            deposit_to_account(balances, loans, id, amount);
        } else {
            withdraw_from_account(balances, loans, id, amount);
        }
        cout << fixed << setprecision(2);
        for (int i = 0; i < CUSTOMER_COUNT; ++i) {
            cout << "BEFORE|" << i << '|' << before[i] << '\n';
        }
        print_accounts(names, balances, loans, CUSTOMER_COUNT);
    }
    delete[] before;
    cout << "FINAL\n";
    print_accounts(names, balances, loans, CUSTOMER_COUNT);
    return 0;
}
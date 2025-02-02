#include<bits/stdc++.h>
using namespace std;

int number_of_suppliers;
vector<int>supply_volume(number_of_suppliers);
int number_of_potential_sites;
vector<vector<int>>potential_sites(number_of_potential_sites, vector<int>(2, 0));
vector<vector<int>>cost_of_supplying(number_of_suppliers, vector<int>(number_of_potential_sites));
vector<int>supply_taken(number_of_suppliers, 0);
int p;

vector<vector<int>>greedy(vector<vector<int>>& allocations) {
    //allocations[i][j]-> if(jth supplier is assigned to ith plant )then allocations[i][j]=1; else 0
    //potential_sites[i][0] contain capacity requirement of potential_sites[i]
    //supply_volume[i] contains volume supplied by ith supplier
    //cost_of_supplying[i][j] contains cost of supplying ith supplier to jth site
}

pair<double, vector<int>> pisingerMinimizeCost(vector<double>& costs, vector<int>& values, int threshold) {
    int n = costs.size();
    int maxValue = 0;

    // Calculate the maximum possible value
    for (int value : values) {
        maxValue += value;
    }

    // Initialize DP table with high cost (INT_MAX for unachievable states)
    vector<double> dp(maxValue + 1, numeric_limits<double>::infinity());
    dp[0] = 0.0;

    // Fill the DP table
    for (int i = 0; i < n; i++) {
        for (int v = maxValue; v >= values[i]; v--) {
            if (dp[v - values[i]] != numeric_limits<double>::infinity()) {
                dp[v] = min((double)dp[v], dp[v - values[i]] + costs[i]);
            }
        }
    }

    // Find the minimum cost to satisfy the threshold value
    double minCost = numeric_limits<double>::infinity();
    int targetValue = -1;
    for (int v = threshold; v <= maxValue; v++) {
        if (dp[v] < minCost) {
            minCost = dp[v];
            targetValue = v;
        }
    }

    // Handle no feasible solution
    if (minCost == numeric_limits<double>::infinity()) {
        cout << "No feasible solution exists.\n";
        return { numeric_limits<double>::infinity(), {} };
    }

    // Trace back to find selected items
    vector<int> selected_items;
    int remainingValue = targetValue;
    for (int i = n - 1; i >= 0 && remainingValue > 0; i--) {
        if (remainingValue >= values[i] && dp[remainingValue] == dp[remainingValue - values[i]] + costs[i]) {
            selected_items.push_back(i);
            remainingValue -= values[i];
        }
    }

    return { minCost, selected_items };
}

pair<double, vector<pair<int, vector<int>>>> subgradient_deflection_Method(
    function<pair<double, vector<pair<int, vector<int>>>>(vector<double>)> objective, // Objective function
    function<vector<double>(vector<double>, pair<double, vector<pair<int, vector<int>>>>)> subgradient, // Subgradient function
    vector<double> x0, // Initial guess
    //double step_size, // Initial step size
    double tolerance, // Convergence tolerance
    int max_iter, // Maximum number of iterations
    double upper_bound,
    double& f_best,
    vector<pair<int, vector<int>>>& ans_second
) {
    //vector<pair<int, vector<int>>> ans_second;
    vector<double> x = x0;
    // Best objective value found
    double prev_f_val = numeric_limits<double>::infinity(); // Best objective value found

    int iter = 0;
    double theta = 1;



    //f_best = 0;
    prev_f_val = upper_bound;
    vector<double>prev_h(number_of_suppliers);
    vector<double>h(number_of_suppliers);
    while (iter < max_iter) {
        // Compute objective value
        pair<double, vector<pair<int, vector<int>>>>obj = objective(x);
        double f_val = obj.first;
        if (f_val > f_best) {
            f_best = f_val;
            ans_second = obj.second;
        }
        f_best = max(f_best, f_val);

        // Check convergence
        if (abs(f_val - prev_f_val) / max(1.0, abs(f_val)) < tolerance) {
            cout << "Converged in " << iter << " iterations." << endl;
            return { f_best,ans_second };
        }

        // Compute subgradient
        vector<double> g = subgradient(x, obj);
        for (int i = 0;i < number_of_suppliers;i++) {
            h[i] = (g[i] + 0.1 * prev_h[i] + 0.3 * h[i]) / 1.4;
        }
        double norm = 0.0;
        for (int i = 0;i < number_of_suppliers;i++)norm += g[i] * g[i];
        double st = theta * (upper_bound - f_val) / norm;
        // Update solution
        for (size_t i = 0; i < x.size(); i++) {
            x[i] = max((double)0, x[i] + st * h[i]);
            //x[i] -= step_size * g[i];
        }

        if (iter % 20 == 0)theta /= 2;
        prev_f_val = f_val;
        upper_bound = min(upper_bound, f_val);
        iter++;
        if (iter == max_iter) {
            cout << "Reached maximum iterations without full convergence." << endl;

            return { f_best,ans_second };
        }
    }
    return { f_best,{{-1,{-1}} } };
}



pair<double, vector<pair<int, vector<int>>>> subgradientMethod(
    function<pair<double, vector<pair<int, vector<int>>>>(vector<double>)> objective, // Objective function
    function<vector<double>(vector<double>, pair<double, vector<pair<int, vector<int>>>>)> subgradient, // Subgradient function
    vector<double> x0, // Initial guess
    //double step_size, // Initial step size
    double tolerance, // Convergence tolerance
    int max_iter // Maximum number of iterations
) {
    vector<pair<int, vector<int>>> ans_second;
    vector<double> x = x0;
    double f_best = numeric_limits<double>::infinity(); // Best objective value found
    double prev_f_val = numeric_limits<double>::infinity(); // Best objective value found
    double upper_bound = 0.0;
    int iter = 0;
    double theta = 1;

    for (int i = 0;i < number_of_potential_sites;i++) {
        for (int j = 0;j < number_of_suppliers;j++)upper_bound += cost_of_supplying[j][i];
        upper_bound += potential_sites[i][1];
    }

    f_best = 0;
    prev_f_val = upper_bound;
    while (iter < max_iter) {
        // Compute objective value
        pair<double, vector<pair<int, vector<int>>>>obj = objective(x);
        double f_val = obj.first;
        if (f_val > f_best) {
            f_best = f_val;
            ans_second = obj.second;
        }
        f_best = max(f_best, f_val);

        // Check convergence
        if (abs(f_val - prev_f_val) / max(1.0, abs(f_val)) < tolerance) {
            //cout << "Converged in " << iter << " iterations." << endl;
            return subgradient_deflection_Method(objective, subgradient, x, tolerance, max_iter, upper_bound, f_best, ans_second);
            return { f_best,ans_second };
        }

        // Compute subgradient
        vector<double> g = subgradient(x, obj);

        double norm = 0.0;
        for (int i = 0;i < number_of_suppliers;i++)norm += g[i] * g[i];
        double st = theta * (upper_bound - f_val) / norm;
        // Update solution
        for (size_t i = 0; i < x.size(); i++) {
            x[i] = max((double)0, x[i] + st * g[i]);
            //x[i] -= step_size * g[i];
        }

        if (iter % 20 == 0)theta /= 2;
        prev_f_val = f_val;
        upper_bound = min(upper_bound, f_val);
        iter++;
        if (iter == max_iter) {
            //cout << "Reached maximum iterations without full convergence." << endl;
            return subgradient_deflection_Method(objective, subgradient, x, tolerance, max_iter, upper_bound, f_best, ans_second);
            return { f_best,ans_second };
        }
    }
    return { f_best,{{-1,{-1}} } };
}


// Objective function
pair<double, vector<pair<int, vector<int>>>> objective(const vector<double>& x) {
    vector <pair< pair<double, vector<int>>, int>>knapsacks(number_of_potential_sites);
    for (int i = 0;i < number_of_potential_sites;i++) {
        vector<double>costs(number_of_suppliers);
        for (int j = 0;j < number_of_suppliers;j++) {
            costs[j] = cost_of_supplying[j][i] + x[j];
        }
        knapsacks[i].first = pisingerMinimizeCost(costs, supply_volume, potential_sites[i][0]);
        knapsacks[i].first.first += potential_sites[i][1];
        knapsacks[i].second = i;
    }

    sort(knapsacks.begin(), knapsacks.end());




    pair<double, vector<pair<int, vector<int>>>>ans;//zlr, site id, supplies to site

    ans.first = -accumulate(x.begin(), x.end(), 0.0);
    for (int i = 0;i < p;i++) {
        ans.first += knapsacks[i].first.first;
        ans.second.push_back({ knapsacks[i].second,knapsacks[i].first.second });
        if (knapsacks[i].first.first == numeric_limits<double>::infinity()) {
            ans.first = numeric_limits<double>::infinity();
        }
    }
    cout << ans.first << " " << endl;
    for (auto& y : x)cout << y << ' ';
    cout << endl;
    return ans;
    //return abs(x[0]) + abs(x[1]);
}

// Subgradient function
vector<double> subgradient(const vector<double>& x, const pair<double, vector<pair<int, vector<int>>>>& obj) {
    vector<double> g(x.size(), -1);
    for (size_t i = 0; i < p; i++) {
        for (size_t j = 0;j < obj.second[i].second.size();j++) {
            g[obj.second[i].second[j]]++;
        }
    }
    return g;
}

void lagrange_relaxation(
    function<pair<double, vector<pair<int, vector<int>>>>(vector<double>)> objective, // Objective function
    function<vector<double>(vector<double>, pair<double, vector<pair<int, vector<int>>>>)> subgradient, // Subgradient function
    //double step_size, // Initial step size
    double tolerance, // Convergence tolerance
    int max_iter
) {
    for (auto& x : supply_taken)x = 0;
    vector<double>x0(number_of_suppliers, 0);
    pair<double, vector<pair<int, vector<int>>>>ans = subgradientMethod(objective, subgradient, x0, tolerance, 10);

    cout << "Minimum Cost: " << ans.first << endl;
    vector<vector<int>>sites_and_suppliers(number_of_potential_sites, vector<int>(number_of_suppliers));
    for (auto& x : ans.second) {
        for (auto& y : x.second) {
            sites_and_suppliers[x.first][y] = 1;
        }
    }
    vector<vector<int>>new_sites_and_suppliers = greedy(sites_and_suppliers);
    for (auto& x : sites_and_suppliers) {
        for (auto& y : x)cout << y << ' ';
        cout << endl;
    }
}



int main() {
    // Example usage
    vector<double> costs = { 1.0,2.0 };    // Cost of each item
    vector<int> values = { 1,2 };   // Value of each item
    int threshold = 2;                   // Minimum threshold value

    auto result = pisingerMinimizeCost(costs, values, threshold);
    double minCost = result.first;
    vector<int> selected_items = result.second;


    cout << minCost << endl;
    for (int item : selected_items) {
        cout << item << " ";
    }
    cout << endl;

    cout << "Enter Number Of Suppliers (0 to 32): ";
    cin >> number_of_suppliers;
    cout << "Enter Number Of potential Sites: ";
    cin >> number_of_potential_sites;
    supply_volume.resize(number_of_suppliers);
    potential_sites.resize(number_of_potential_sites, vector<int>(2));
    cost_of_supplying.resize(number_of_suppliers, vector<int>(number_of_potential_sites));
    supply_taken.resize(number_of_suppliers);


    cout << "Enter supply volume for each supplier: ";
    for (auto& x : supply_volume)cin >> x;
    cout << "Enter requirement and cost of each potential site: ";
    for (auto& x : potential_sites)cin >> x[0] >> x[1];
    cout << "Enter cost of supplying for each supplier and potential site pair: ";
    for (auto& x : cost_of_supplying)
        for (auto& y : x)cin >> y;
    cout << "Enter number of facilities to open: ";
    cin >> p;

    int tolerance = 0.01;
    lagrange_relaxation(objective, subgradient, tolerance, 1000);
    // if (ans == INT_MAX) {
    //     cout << "No distribution can satisfy demand of p potential sites\n";
    // }
    // else cout << ans << endl;

    return 0;
}
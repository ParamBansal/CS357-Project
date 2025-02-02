#include<bits/stdc++.h>
using namespace std;
int number_of_suppliers;
vector<int>supply_volume(number_of_suppliers);
int number_of_potential_sites;
vector<vector<int>>potential_sites(number_of_potential_sites, vector<int>(2, 0));
vector<vector<int>>cost_of_supplying(number_of_suppliers, vector<int>(number_of_potential_sites));
vector<int>supply_taken(number_of_suppliers, 0);
int naive(int p,int i) {
    if (p == 0)return 0;
    if (i < 0)return INT_MAX;
    int notake = naive(p, i - 1);
    if (potential_sites[i][0] == 0) {
        int taken = naive(p - 1, i - 1);
        if (taken != INT_MAX)return min(notake, potential_sites[i][1] + taken);
        return notake;
    }
    int take = INT_MAX;
    for (int j = 0;j < (1<<number_of_suppliers);j++) {
        vector<int>original = supply_taken;
        int s_cost = 0, s_volume = 0;
        bool not_optimal = 0;
        for (int x = 0;x < 32;x++) {
            if ((j) & (1 << x)) {
                if (supply_taken[x]) {
                    not_optimal = 1;
                    break;
                }
                s_cost += cost_of_supplying[x][i];
                s_volume += supply_volume[x];
                supply_taken[x] = 1;
            }
        }

        if (not_optimal) {
            supply_taken = original;
            continue;
        }
        if (s_volume >= potential_sites[i][0]) {
            int temp = naive(p - 1, i - 1);
            if (temp == INT_MAX) {
                supply_taken = original;
                continue;
            }
            take = min(take, potential_sites[i][1] + s_cost + temp);
        }
        supply_taken = original;
    }
    return min(take, notake);
}
int main() {
    cout << "Enter Number Of Suppliers (0 to 32): ";
    cin >> number_of_suppliers;
    cout << "Enter Number Of potential Sites: ";
    cin >> number_of_potential_sites;
    supply_volume.resize(number_of_suppliers);
    potential_sites.resize(number_of_potential_sites, vector<int>(2));
    cost_of_supplying.resize(number_of_suppliers, vector<int>(number_of_potential_sites));
    supply_taken.resize(number_of_suppliers);

    for (auto& x : supply_taken)x = 0;
    cout << "Enter supply volume for each supplier: ";
    for (auto& x : supply_volume)cin >> x;
    cout << "Enter requirement and cost of each potential site: ";
    for (auto& x : potential_sites)cin >> x[0] >> x[1];
    cout << "Enter cost of supplying for each supplier and potential site pair: ";
    for (auto& x : cost_of_supplying)
        for (auto& y : x)cin >> y;
    cout << "Enter number of facilities to open: ";
    int p;cin >> p;
    int ans = naive(p, number_of_potential_sites - 1);
    if (ans == INT_MAX) {
        cout << "No distribution can satisfy demand of p potential sites\n";
    }
    else cout << ans << endl;
    return 0;
}